#include "NW_Device.h"

bool NW_Device::begin(uint8_t address, const char* name, uint8_t minPatch, unsigned long bootTimeoutMs) {
  _adr = address;
  Wire.begin();

  // Check ACK, retrying for the device's boot time if asked to.
  unsigned long t0 = millis();
  while (true) {
    Wire.beginTransmission(_adr);
    if (Wire.endTransmission() == 0) break;
    if (millis() - t0 >= bootTimeoutMs) { _beginFailure = 1; return false; }
    delay(1);
  }

  // Page 0 Blocks 0-1 (0x00-0x0F): schema, name, versions. Read in one
  // transaction; the firmware serves Page 0 from EEPROM so it is valid before
  // the first reading.
  uint8_t p0[16];
  _hwMajor = _hwMinor = _fwPatch = 0;
  if (!readBytes(NW_REG_SCHEMA, p0, 16)) { _beginFailure = 2; return false; }
  _hwMajor = p0[NW_REG_HW_MAJOR];
  _hwMinor = p0[NW_REG_HW_MINOR];
  _fwPatch = p0[NW_REG_FW_PATCH];
  if (p0[NW_REG_SCHEMA] != 0x01) { _beginFailure = 3; return false; }   // not Schema 1 (0x00 legacy, 0xFF unprovisioned, other)
  bool ended = false;                                     // 7-byte name field, null-padded
  for (uint8_t i = 0; i < 7; i++) {
    char expected = ended ? 0 : name[i];
    if (expected == 0) ended = true;
    if (p0[NW_REG_NAME + i] != (uint8_t)expected) { _beginFailure = 4; return false; }
  }
  if (_fwPatch < minPatch) { _beginFailure = 5; return false; }   // register map older than this library
  // Block 0 before any write: the boot reports (unit reset 0xE6, Page 0 check 0xE3)
  // would be cleared by the first trigger, which is a Control write.
  uint8_t b0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (!readBytes(NW_REG_STATUS, b0, 8)) { _beginFailure = 2; return false; }
  _report.status = b0[0];
  _report.code   = b0[7];
  _bootReport = _report;               // kept for the logger's status file until it clears it
  _beginFailure = 0;
  return true;
}

String NW_Device::beginFailure() const {
  switch (_beginFailure) {
    case 1: return String(F("NotAnswering"));
    case 2: return String(F("ReadFailed"));
    case 3: return String(F("NotSchema1"));
    case 4: return String(F("WrongName"));
    case 5: return String(F("OldFirmware"));
    default: return String(F("None"));
  }
}

bool NW_Device::readBytes(uint8_t reg, uint8_t* buf, uint8_t n) {
  // One transaction per chunk: pointer write, then requestFrom. Schema 1 firmware
  // serves up to 32 bytes with auto-increment, and the AVR Wire buffer is 32.
  while (n > 0) {
    uint8_t k = (n > NW_WIRE_CHUNK) ? NW_WIRE_CHUNK : n;
    Wire.beginTransmission(_adr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) return false;
    if (Wire.requestFrom(_adr, k) != k) return false;
    for (uint8_t i = 0; i < k; i++) buf[i] = Wire.read();
    buf += k; reg += k; n -= k;
  }
  return true;
}

bool NW_Device::readData(uint8_t reg, uint8_t* buf, uint8_t n) {
  // Seqlock over the bus: the counter after the read must equal the one captured
  // before it. A device that committed in between gets captured again and re-read.
  _dataMoved = false;
  for (uint8_t attempt = 0; ; attempt++) {
    if (!readBytes(reg, buf, n)) return false;
    if (readCounter() == _lastCounter) return true;
    if (attempt >= NW_DATA_RETRIES) { _dataMoved = true; return false; }
    if (!captureReading()) return false;
  }
}

bool NW_Device::writeByte(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(_adr);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool NW_Device::setI2CAddress(uint8_t newAddress) { return writeByte(NW_REG_I2C_ADDR, newAddress); }

static size_t printHex(Print& out, const uint8_t* b, uint8_t n) {
  static const char digits[] = "0123456789ABCDEF";
  size_t k = 0;
  for (uint8_t i = 0; i < n; i++) { k += out.print(digits[b[i] >> 4]); k += out.print(digits[b[i] & 0x0F]); }
  return k;
}

size_t NW_Device::printSnapshot(Print& out, const char* const* chipNames, uint8_t nChips, bool boot) {
  const NW_Report& r = boot ? _bootReport : _report;
  uint8_t page[32];
  size_t n = 0;
  if (!readBytes(0x00, page, 32)) return out.print(F("NotAnswering"));
  for (uint8_t i = 1; i <= 7 && page[i]; i++) n += out.print((char)page[i]);   // name
  n += out.print(',');
  for (uint8_t i = 0; i < 4; i++) { if (i) n += out.print('-'); n += printHex(out, page + 0x10 + 2 * i, 2); }   // serial, Block 2
  n += out.print(','); n += out.print(page[NW_REG_HW_MAJOR]); n += out.print('.'); n += out.print(page[NW_REG_HW_MINOR]);   // HW version
  n += out.print(','); n += out.print(page[NW_REG_FW_PATCH]);                                                               // FW patch
  n += out.print(F(",0x")); n += printHex(out, &r.code, 1);
  n += out.print(','); n += out.print(r.note(chipNames, nChips));
  n += out.print(','); n += printHex(out, page, 32);                                   // Page 0
  for (uint8_t p = 0x20; p <= 0x40; p += 0x20) {                                       // Page 1 (calibration), Page 2 (data)
    n += out.print(',');
    if (readBytes(p, page, 32)) n += printHex(out, page, 32); else n += out.print(F("NotAnswering"));
  }
  return n;
}

uint8_t NW_Device::readConfig() {
  uint8_t v = 0xFF;
  readBytes(NW_REG_CONFIG, &v, 1);
  return v;
}

bool NW_Device::ready() {
  uint8_t status = 0;
  return readBytes(NW_REG_STATUS, &status, 1) && (status & NW_BIT_READY);
}

uint16_t NW_Device::readCounter() {
  uint8_t d[2] = {0xFF, 0xFF};
  readBytes(NW_REG_COUNTER, d, 2);
  return (uint16_t)((d[1] << 8) | d[0]);
}

bool NW_Device::newReading() { return readCounter() != _lastCounter; }

bool NW_Device::requestReading(uint8_t chips) {
  _counterBefore = readCounter();
  _chips = chips;
  return writeByte(NW_REG_CTRL, (uint8_t)(NW_CTRL_TRIGGER | (chips << 1)));
}

bool NW_Device::waitReading() {
  // Without a request, wait for the counter to move past the last capture
  // (a free-running device); with one, past the value seen at the request.
  uint16_t before = (_counterBefore != 0xFFFF) ? _counterBefore : _lastCounter;
  unsigned long start = millis();
  while (millis() - start < _timeout) {
    uint16_t now = readCounter();
    if (now != before) { _lastCounter = now; _counterBefore = 0xFFFF; return true; }
    delay(1);
  }
  return false;
}

bool NW_Device::captureReading() {
  // Block 0 of the new reading: status (0x40) and report (0x47), one 8-byte read.
  uint8_t b0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (!readBytes(NW_REG_STATUS, b0, 8)) return false;
  _report.status = b0[0];
  _report.code   = b0[7];
  _lastCounter  = (uint16_t)((b0[3] << 8) | b0[2]);       // the counter of the reading captured
  // A selected chip that reports "no acknowledge" or "not initialised" is not
  // coming for the rest of this batch (NW-Device-Specification, Readings requested).
  uint8_t chip = _report.chip();
  if (chip < 6 && (_chips & (1 << chip)) && _report.chipFaulted(chip) && _report.chipAbsent()) _absentChips |= (1 << chip);
  return true;
}

bool NW_Device::takeReading(uint8_t chips) {
  if (!requestReading(chips)) return false;
  if (!waitReading()) return false;
  return captureReading();
}

bool NW_Device::writeBatch(uint16_t n) {
  _absentChips = 0;
  return writeByte(NW_REG_REQUEST, n & 0xFF) && writeByte(NW_REG_REQUEST + 1, n >> 8);
}

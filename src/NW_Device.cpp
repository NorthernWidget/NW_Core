#include "NW_Device.h"

bool NW_Device::begin(uint8_t address, const char* name, uint8_t minPatch, unsigned long bootTimeoutMs) {
  _adr = address;
  Wire.begin();

  // Check ACK, retrying for the device's boot time if asked to.
  unsigned long t0 = millis();
  while (true) {
    Wire.beginTransmission(_adr);
    if (Wire.endTransmission() == 0) break;
    if (millis() - t0 >= bootTimeoutMs) return false;
    delay(1);
  }

  // Page 0 Blocks 0-1 (0x00-0x0F): schema, name, versions. Read in one
  // transaction; the firmware serves Page 0 from EEPROM so it is valid before
  // the first reading.
  uint8_t p0[16];
  _hwMajor = _hwMinor = _fwPatch = 0;
  if (!readBytes(NW_REG_SCHEMA, p0, 16)) return false;
  _hwMajor = p0[NW_REG_HW_MAJOR];
  _hwMinor = p0[NW_REG_HW_MINOR];
  _fwPatch = p0[NW_REG_FW_PATCH];
  if (p0[NW_REG_SCHEMA] != 0x01) return false;            // not Schema 1 (0x00 legacy, 0xFF unprovisioned, other)
  bool ended = false;                                     // 7-byte name field, null-padded
  for (uint8_t i = 0; i < 7; i++) {
    char expected = ended ? 0 : name[i];
    if (expected == 0) ended = true;
    if (p0[NW_REG_NAME + i] != (uint8_t)expected) return false;
  }
  if (_fwPatch < minPatch) return false;                  // register map older than this library
  return true;
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

bool NW_Device::writeByte(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(_adr);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool NW_Device::setI2CAddress(uint8_t newAddress) { return writeByte(NW_REG_I2C_ADDR, newAddress); }

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
  // Block 0 of the new reading: status (0x20) and latched fault (0x27), one 8-byte read.
  uint8_t b0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (!readBytes(NW_REG_STATUS, b0, 8)) return false;
  _fault.status = b0[0];
  _fault.code   = b0[7];
  _lastCounter  = (uint16_t)((b0[3] << 8) | b0[2]);       // the counter of the reading captured
  // A selected chip that reports "no acknowledge" or "not initialised" is not
  // coming for the rest of this batch (NW-Device-Specification, Readings requested).
  uint8_t chip = _fault.chip();
  if (chip < 6 && (_chips & (1 << chip)) && _fault.chipFaulted(chip) && _fault.chipAbsent()) _absentChips |= (1 << chip);
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

// Shared support for the host-side harnesses of every NW library: the Page 0
// image a provisioned device serves, the firmware emulation that answers a
// trigger, a CRC, and a Print that writes into a buffer. Include after the
// stubs (Arduino.h, Wire.h) and the library's .cpp; TwoWire Wire must exist.
#ifndef NW_TestSupport_h
#define NW_TestSupport_h

#include <functional>
#include <string.h>

// CRC-8/SMBUS, as NW-Provision writes it into Page 0 byte 0x1E.
static uint8_t crc8(const uint8_t* d, uint8_t n) {
  uint8_t c = 0; for (uint8_t i = 0; i < n; i++) { c ^= d[i]; for (int b = 0; b < 8; b++) c = (c & 0x80) ? (c << 1) ^ 0x07 : (c << 1); }
  return c;
}

// Zero the register image and write Page 0 as NW-Provision does (schema, name,
// HW 0.<hwMinor>, the firmware's patch at 0x0A, board type <address>.<hwMinor>,
// group 7, unit 42, magic, CRC, address) plus Block 0 with a completed reading
// (ready, both chips selected, counter 1). The caller adds its Page 1 data.
static void nwLoadPage0(uint8_t* r, const char* name, uint8_t address, uint8_t hwMinor, uint8_t fwPatch, uint8_t schema) {
  memset(r, 0, 128);
  r[0x00] = schema; for (int i = 0; i < 7 && name[i]; i++) r[0x01 + i] = name[i];
  r[0x08] = 0; r[0x09] = hwMinor; r[0x0A] = fwPatch;
  r[0x10] = address; r[0x11] = hwMinor; r[0x12] = 0; r[0x13] = 7; r[0x14] = 0; r[0x15] = 42;
  r[0x1D] = 0x4E; r[0x1E] = crc8(r, 0x1E); r[0x1F] = address;
  r[0x20] = 0x01;                                                   // ready
  r[0x21] = 0x06;                                                   // both chips selected
  r[0x22] = 1; r[0x23] = 0;                                         // reading counter = 1
  r[0x26] = 0x00; r[0x27] = 0x00;
}

// Emulate the Schema 1 firmware's response to a control write: a trigger
// completes a reading at once (counter +1, ready set, trigger and sleep bits
// cleared, fault byte cleared). A per-test hook (onReading) can vary the data
// before the counter moves; lastRequest holds the readings-requested word the
// stub firmware last saw.
static std::function<void(TwoWire&)> onReading;
static uint16_t lastRequest = 0;
static void installFirmwareEmulation() {
  Wire.onWrite = [](TwoWire& w, uint8_t reg, uint8_t val) {
    if (reg == 0x24) lastRequest = (lastRequest & 0xFF00) | val;
    if (reg == 0x25) lastRequest = (lastRequest & 0x00FF) | (val << 8);
    if (reg != 0x21) return;
    w.image[0x27] = 0;                                  // any control write acknowledges the fault
    if (!(val & 0x01)) return;
    w.image[0x21] = val & 0x7E;                         // trigger and sleep consumed
    if (onReading) onReading(w);
    uint16_t c = w.image[0x22] | (w.image[0x23] << 8); c++;
    w.image[0x22] = c & 0xFF; w.image[0x23] = c >> 8;
    w.image[0x20] |= 0x01;
  };
}

// Print into a fixed buffer: the in-memory Print destination from the design.
class BufferPrint : public Print {
  char* _buf; size_t _cap, _len = 0;
  public:
  BufferPrint(char* buf, size_t cap) : _buf(buf), _cap(cap) { _buf[0] = 0; }
  size_t write(uint8_t c) override { if (_len + 1 >= _cap) return 0; _buf[_len++] = c; _buf[_len] = 0; return 1; }
  size_t length() const { return _len; }
};

#endif

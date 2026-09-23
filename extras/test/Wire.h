// Stub TwoWire serving a 128-byte register image with the transaction semantics
// an NW Schema 1 device presents: beginTransmission(adr); write(reg) sets the
// pointer; a second write() is a register write; endTransmission() returns 0
// when the address matches (and the device is present); requestFrom(adr, n)
// queues up to 32 bytes from the pointer with auto-increment (the AVR Wire
// buffer); read() pops, or returns -1 (0xFF when cast) when nothing is queued.
// Hooks let a test emulate firmware: onWrite(wire, reg, value) after a register
// write; beforeRead(wire, pointer) before bytes are queued. A test sets
// freeRunPeriodMs to emulate a device that completes readings on its own.
#pragma once
#include <cstdint>
#include <cstring>
#include <deque>
#include <functional>

class TwoWire {
  public:
  uint8_t image[128] = {0};
  uint8_t deviceAddress = 0x00;           // the harness sets the device under test's address
  bool present = true;
  uint32_t presentAfterMs = 0;             // no ACK before this millis() (boot emulation)
  uint32_t freeRunPeriodMs = 0;            // >0: counter (0x22-0x23) advances every period, ready set
  uint32_t _lastFreeRun = 0;
  std::function<void(TwoWire&, uint8_t)> beforeRead;
  std::function<void(TwoWire&, uint8_t, uint8_t)> onWrite;
  unsigned transactions = 0;               // requestFrom calls
  unsigned ackAttempts = 0;                // address-only transmissions (ACK tests)

  void begin() {}
  void beginTransmission(uint8_t adr) { _adr = adr; _nwrites = 0; }
  size_t write(uint8_t v) {
    if (_nwrites == 0) _ptr = v;
    else if (_present()) { image[_ptr & 0x7F] = v; if (onWrite) onWrite(*this, _ptr, v); _ptr++; }
    _nwrites++; return 1;
  }
  uint8_t endTransmission(bool = true) { if (_nwrites == 0) ackAttempts++; return _present() ? 0 : 2; }
  uint8_t requestFrom(uint8_t adr, uint8_t n) {
    transactions++;
    if (adr != deviceAddress || !_present()) return 0;
    _freeRun();
    if (beforeRead) beforeRead(*this, _ptr);
    uint8_t k = n > 32 ? 32 : n;                          // the AVR Wire buffer
    for (uint8_t i = 0; i < k; i++) _q.push_back(image[_ptr++ & 0x7F]);
    return k;
  }
  uint8_t requestFrom(int adr, int n) { return requestFrom((uint8_t)adr, (uint8_t)n); }
  int read() { if (_q.empty()) return -1; int v = _q.front(); _q.pop_front(); return v; }
  int available() { return (int)_q.size(); }
  uint16_t counter() const { return image[0x22] | (image[0x23] << 8); }
  void bumpCounter() { uint16_t c = counter() + 1; image[0x22] = c & 0xFF; image[0x23] = c >> 8; image[0x20] |= 0x01; }
  private:
  bool _present() { return present && _adr == deviceAddress && millis() >= presentAfterMs; }
  void _freeRun() {
    if (!freeRunPeriodMs) return;
    while (millis() - _lastFreeRun >= freeRunPeriodMs) { _lastFreeRun += freeRunPeriodMs; bumpCounter(); }
  }
  uint8_t _adr = 0, _ptr = 0; unsigned _nwrites = 0; std::deque<uint8_t> _q;
};
extern TwoWire Wire;

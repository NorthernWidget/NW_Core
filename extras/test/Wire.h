// Stub TwoWire serving a 128-byte register image with the transaction semantics
// an NW Schema 1 device presents: beginTransmission(adr); write(reg) sets the
// pointer; a second write() is a register write; endTransmission() returns 0
// when the address matches (and the device is present); requestFrom(adr, n)
// queues up to 32 bytes from the pointer with auto-increment (the AVR Wire
// buffer); read() pops, or returns -1 (0xFF when cast) when nothing is queued.
// The stub is the bus; the device is a hook. onWrite(wire, reg, value) runs
// after a register write (emulated firmware); beforeRead(wire, pointer) runs
// before bytes are queued from the image; onRequest(wire, n, out) replaces the
// image altogether and supplies the n bytes a read returns, for a device that
// is not a register file (the T9602 answers every read with the same four
// status-and-data bytes; a command-response chip would decode the last write);
// it returns false to hand a read back to the image (one chip of two).
// A test sets freeRunPeriodMs to emulate a device that completes readings on
// its own. A harness with two chips on one bus (Libelle v1: the pyranometer
// bridge and the ADXL343) sets isPresent for both addresses and branches on
// address() inside its hooks.
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
  std::function<bool(TwoWire&, uint8_t, std::deque<uint8_t>&)> onRequest;   // the device answers a read itself; return false to fall back to the image
  std::function<bool(uint8_t)> isPresent;  // which addresses acknowledge; default: deviceAddress alone (a harness with two chips on the bus branches on address())
  std::function<bool(uint8_t)> hookOnly;   // addresses whose register writes never touch the image (a second chip served entirely by the hooks)
  std::function<void(TwoWire&, uint8_t, uint8_t)> onWrite;
  unsigned transactions = 0;               // requestFrom calls
  unsigned ackAttempts = 0;                // address-only transmissions (ACK tests)

  void begin() {}
  void beginTransmission(uint8_t adr) { _adr = adr; _nwrites = 0; }
  size_t write(uint8_t v) {
    if (_nwrites == 0) _ptr = v;
    else if (_present()) { if (!(hookOnly && hookOnly(_adr))) image[_ptr & 0x7F] = v; if (onWrite) onWrite(*this, _ptr, v); _ptr++; }
    _nwrites++; return 1;
  }
  uint8_t endTransmission(bool = true) { if (_nwrites == 0) ackAttempts++; return _present() ? 0 : 2; }
  uint8_t requestFrom(uint8_t adr, uint8_t n) {
    transactions++;
    _adr = adr;
    if (!_present()) return 0;
    _freeRun();
    uint8_t k = n > 32 ? 32 : n;                          // the AVR Wire buffer
    if (onRequest && onRequest(*this, k, _q)) return n;
    if (beforeRead) beforeRead(*this, _ptr);
    for (uint8_t i = 0; i < k; i++) _q.push_back(image[_ptr++ & 0x7F]);
    return k;
  }
  uint8_t requestFrom(int adr, int n) { return requestFrom((uint8_t)adr, (uint8_t)n); }
  int read() { if (_q.empty()) return -1; int v = _q.front(); _q.pop_front(); return v; }
  int available() { return (int)_q.size(); }
  uint16_t counter() const { return image[0x22] | (image[0x23] << 8); }
  void bumpCounter() { uint16_t c = counter() + 1; image[0x22] = c & 0xFF; image[0x23] = c >> 8; image[0x20] |= 0x01; }
  private:
  uint8_t address() const { return _adr; }  // the address of the transaction in progress, for hooks serving more than one device
  uint8_t pointer() const { return _ptr; }  // the register pointer the last write set, for hooks that serve a register file of their own
  bool _present() { return present && (isPresent ? isPresent(_adr) : _adr == deviceAddress) && millis() >= presentAfterMs; }
  void _freeRun() {
    if (!freeRunPeriodMs) return;
    while (millis() - _lastFreeRun >= freeRunPeriodMs) { _lastFreeRun += freeRunPeriodMs; bumpCounter(); }
  }
  uint8_t _adr = 0, _ptr = 0; unsigned _nwrites = 0; std::deque<uint8_t> _q;
};
extern TwoWire Wire;

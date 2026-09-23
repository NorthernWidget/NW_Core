// Output-regression test for NW_Core: compiles the library on the host against
// the stubs here and prints what NW_Device, NW_Fault, and NW_Readings do for
// fixed device images and emulated firmware. run.sh diffs against baseline.txt.
#include "Arduino.h"
#include "Wire.h"
TwoWire Wire;
#include "../../src/NW_Core.h"
#include "../../src/NW_Device.cpp"

static uint8_t crc8(const uint8_t* d, uint8_t n) {           // CRC-8/SMBUS, as NW-Provision writes it
  uint8_t c = 0; for (uint8_t i = 0; i < n; i++) { c ^= d[i]; for (int b = 0; b < 8; b++) c = (c & 0x80) ? (c << 1) ^ 0x07 : (c << 1); }
  return c;
}
// A provisioned Schema 1 device named "Apis", HW 0.1, given firmware patch, a completed reading.
static void loadImage(uint8_t fwPatch = 2, uint8_t schema = 0x01, const char* name = "Apis") {
  Wire = TwoWire(); uint8_t* r = Wire.image;
  r[0x00] = schema; for (int i = 0; i < 7 && name[i]; i++) r[0x01 + i] = name[i];
  r[0x08] = 0; r[0x09] = 1; r[0x0A] = fwPatch;
  r[0x10] = 0x41; r[0x11] = 0x01; r[0x12] = 0; r[0x13] = 7; r[0x14] = 0; r[0x15] = 42;
  r[0x1D] = 0x4E; r[0x1E] = crc8(r, 0x1E); r[0x1F] = 0x41;
  r[0x20] = 0x01; r[0x21] = 0x06; r[0x22] = 1; r[0x23] = 0;
  _millis_counter() = 0;
}
// Emulated firmware: a trigger completes a reading at once.
static void firmware() {
  Wire.onWrite = [](TwoWire& w, uint8_t reg, uint8_t val) {
    if (reg == 0x21 && (val & 0x01)) { w.image[0x21] = val & ~0x01; w.image[0x27] = 0; w.bumpCounter(); }
    if (reg == 0x21) w.image[0x27] = 0;                  // any control write clears the latched fault
  };
}
class BufferPrint : public Print {
  char* _buf; size_t _cap, _len = 0;
  public:
  BufferPrint(char* buf, size_t cap) : _buf(buf), _cap(cap) { _buf[0] = 0; }
  size_t write(uint8_t c) override { if (_len + 1 >= _cap) return 0; _buf[_len++] = c; _buf[_len] = 0; return 1; }
};
static const char* kindText(const NW_Fault& f) { static char b[40]; BufferPrint bp(b, sizeof b); f.printKind(bp); return b; }

int main() {
  // 1. begin() gates
  { loadImage(); NW_Device d; bool ok = d.begin(0x41, "Apis", 2);
    printf("[begin] ok=%d hw=%u.%u fw=%u attempts=%u\n", ok, d.hardwareMajor(), d.hardwareMinor(), d.firmwareVersion(), Wire.ackAttempts); }
  { loadImage(2, 0x01, "Apix"); NW_Device d; bool ok = d.begin(0x41, "Apis", 2); printf("[begin] wrong name: ok=%d fw=%u (versions stored before refusal)\n", ok, d.firmwareVersion()); }
  { loadImage(2, 0x01, "Api");  NW_Device d; printf("[begin] name 'Api' vs 'Apis': ok=%d\n", d.begin(0x41, "Apis", 2)); }
  { loadImage(2, 0x00);         NW_Device d; printf("[begin] schema 0x00: ok=%d\n", d.begin(0x41, "Apis", 2)); }
  { loadImage(1);               NW_Device d; bool ok = d.begin(0x41, "Apis", 2); printf("[begin] patch 1 < min 2: ok=%d fw=%u\n", ok, d.firmwareVersion()); }
  { loadImage(); Wire.present = false; NW_Device d; bool ok = d.begin(0x41, "Apis", 2); printf("[begin] absent, no boot wait: ok=%d attempts=%u\n", ok, Wire.ackAttempts); }
  { loadImage(); Wire.presentAfterMs = 30; NW_Device d; bool ok = d.begin(0x41, "Apis", 2, 100);
    printf("[begin] boots after 30 ms, wait 100: ok=%d attempts=%u took=%u ms\n", ok, Wire.ackAttempts, (unsigned)millis()); }
  { loadImage(); Wire.presentAfterMs = 300; NW_Device d; bool ok = d.begin(0x41, "Apis", 2, 100);
    printf("[begin] boots after 300 ms, wait 100: ok=%d took=%u ms\n", ok, (unsigned)millis()); }

  // 2. Handshake with emulated firmware
  { loadImage(); firmware(); NW_Device d; d.begin(0x41, "Apis", 2);
    printf("[handshake] newReading before any=%d ready=%d\n", d.newReading(), d.ready());
    unsigned tx = Wire.transactions; bool ok = d.takeReading(0x01);
    printf("[handshake] takeReading(chip0): ok=%d counter=%u newReading=%d ctrl=0x%02X fault='%s' transactions=%u\n",
       ok, d.readCounter(), d.newReading(), Wire.image[0x21], kindText(d.fault()), Wire.transactions - tx);
    // split form: request now, wait later
    ok = d.requestReading(0x03); printf("[handshake] request(chips 0,1) ok=%d ctrl written=0x%02X", ok, 0x01 | (0x03 << 1));
    ok = d.waitReading(); printf(" wait=%d", ok); ok = d.captureReading(); printf(" capture=%d status=0x%02X\n", ok, d.fault().status); }

  // 3. Free-running device: no request, the counter moves on its own
  { loadImage(); NW_Device d; d.begin(0x41, "Apis", 2); d.captureReading(); Wire.freeRunPeriodMs = 10;
    bool ok = d.waitReading(); printf("[free-running] waitReading without request: ok=%d took=%u ms counter=%u\n", ok, (unsigned)millis(), d.readCounter()); }

  // 4. Timeout: firmware never answers
  { loadImage(); NW_Device d; d.begin(0x41, "Apis", 2); uint32_t t0 = millis(); bool ok = d.takeReading(0x01);
    printf("[timeout] default: ok=%d after %u ms\n", ok, (unsigned)(millis() - t0));
    d.setTimeout(50); t0 = millis(); ok = d.takeReading(0x01); printf("[timeout] setTimeout(50): ok=%d after %u ms\n", ok, (unsigned)(millis() - t0)); }

  // 5. Faults and batch abandonment
  { loadImage(); firmware(); NW_Device d; d.begin(0x41, "Apis", 2);
    Wire.beforeRead = [](TwoWire& w, uint8_t) { w.image[0x20] = 0x83; w.image[0x27] = 0x02; };   // ready | chip0 fault | pan; chip 0 timeout
    d.takeReading(0x01);
    printf("[fault] faulted(0)=%d faulted(1)=%d any=%d chip=%u kind=%u '%s' batchFaulted=%d\n",
       d.faulted(0), d.faulted(1), d.anyFault(), d.faultChip(), d.faultKind(), kindText(d.fault()), d.batchFaulted());
    Wire.beforeRead = [](TwoWire& w, uint8_t) { w.image[0x20] = 0x83; w.image[0x27] = 0x01; };   // chip 0 no acknowledge
    d.takeReading(0x01); printf("[fault] chip0 no-ack, chip0 selected: batchFaulted=%d '%s'\n", d.batchFaulted(), kindText(d.fault()));
    d.writeBatch(4); printf("[fault] writeBatch(4): batchFaulted=%d word=%u\n", d.batchFaulted(), Wire.image[0x24] | (Wire.image[0x25] << 8));
    Wire.beforeRead = [](TwoWire& w, uint8_t) { w.image[0x20] = 0x85; w.image[0x27] = 0x21; };   // chip 1 no-ack
    d.takeReading(0x01); printf("[fault] chip1 no-ack, only chip0 selected: batchFaulted=%d\n", d.batchFaulted());
    d.takeReading(0x02); printf("[fault] chip1 no-ack, chip1 selected: batchFaulted(any)=%d chip0=%d chip1=%d\n", d.batchFaulted(), d.batchFaulted(0x01), d.batchFaulted(0x02));
    Wire.beforeRead = [](TwoWire& w, uint8_t) { w.image[0x20] = 0x01; w.image[0x27] = 0xE6; };   // unit: reset since configured
    d.resetBatch(); d.takeReading(0x01); printf("[fault] unit: chip=%u isUnit=%d '%s' batchFaulted=%d\n", d.faultChip(), d.fault().isUnit(), kindText(d.fault()), d.batchFaulted());
    Wire.beforeRead = [](TwoWire& w, uint8_t) { w.image[0x20] = 0x01; w.image[0x27] = 0x11; };   // chip 0 kind 17
    d.takeReading(0x01); printf("[fault] device-specific kind: '%s'\n", kindText(d.fault())); Wire.beforeRead = nullptr; }

  // 6. Long reads are chunked at the 32-byte Wire buffer
  { loadImage(); NW_Device d; d.begin(0x41, "Apis", 2); for (int i = 0; i < 128; i++) Wire.image[i] = i;
    uint8_t buf[40] = {0}; unsigned tx = Wire.transactions; bool ok = d.readBytes(0x20, buf, 40);
    printf("[readBytes] 40 bytes from 0x20: ok=%d transactions=%u first=0x%02X last=0x%02X\n", ok, Wire.transactions - tx, buf[0], buf[39]); }

  // 7. Registers: batch word, config, sleep, address
  { loadImage(); NW_Device d; d.begin(0x41, "Apis", 2); d.writeBatch(300); d.writeConfig(0x03); d.sleep(); d.setI2CAddress(0x45);
    printf("[registers] batch=%u config=0x%02X ctrl=0x%02X addr=0x%02X readConfig=0x%02X\n",
       Wire.image[0x24] | (Wire.image[0x25] << 8), Wire.image[0x26], Wire.image[0x21], Wire.image[0x1F], d.readConfig()); }

  // 8. NW_Readings
  { NW_Readings<int16_t, 4> r; printf("[readings] empty: count=%u mean=%.1f\n", r.count(), r.mean());
    int16_t v[] = {10, 20, 30, 40, 50}; for (int16_t x : v) r.append(x);
    printf("[readings] int16 cap 4 after 5 appends: count=%u last=%d mean=%.4f std=%.4f sterr=%.4f median=%.4f\n",
       r.count(), r.last(), r.mean(), r.std(), r.sterr(), r.median());
    r.reset(); r.append(7); printf("[readings] single: count=%u mean=%.4f std=%.4f sterr=%.4f median=%.4f\n", r.count(), r.mean(), r.std(), r.sterr(), r.median());
    NW_Readings<float, 8> f; f.append(1.5f); f.append(10.0f); f.append(2.5f);
    printf("[readings] float 3 values: mean=%.4f median=%.4f last=%.2f capacity=%u\n", f.mean(), f.median(), f.last(), f.capacity());
    f.append(4.0f); printf("[readings] float 4 values: median=%.4f (even count averages the middle two)\n", f.median()); }

  fprintf(stderr, "bus transactions total: %u\n", Wire.transactions);
  return 0;
}

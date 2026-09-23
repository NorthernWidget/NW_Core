// Output-regression test for NW_Core: compiles the library on the host against
// the stubs here and prints what NW_Device, NW_Fault, and NW_Readings do for
// fixed device images and emulated firmware. run.sh diffs against baseline.txt.
#include "Arduino.h"
#include "Wire.h"
TwoWire Wire;
#include "../../src/NW_Core.h"
#include "../../src/NW_Device.cpp"

#include "NW_TestSupport.h"
// A provisioned Schema 1 device named "Apis" at 0x41, HW 0.1, given firmware patch, a completed reading.
static void loadImage(uint8_t fwPatch = 2, uint8_t schema = 0x01, const char* name = "Apis") {
  Wire = TwoWire(); Wire.deviceAddress = 0x41;
  nwLoadPage0(Wire.image, name, 0x41, 1, fwPatch, schema);
  _millis_counter() = 0;
}
// Emulated firmware: a trigger completes a reading at once.
static void firmware() { installFirmwareEmulation(); }
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

  // 6b. readData(): the counter check. A device commits a new reading (data 0xAA, counter +1)
  //     right after the first data read. The plain readBytes pairs the captured counter with
  //     the newer data; readData() sees the counter move, captures again, re-reads, and returns
  //     the reading its fault() describes. Then a device that commits on every transaction:
  //     readData() gives up after NW_DATA_RETRIES re-reads instead of looping.
  { loadImage(); firmware(); NW_Device d; d.begin(0x41, "Apis", 2); d.takeReading(0x01);
    int commits = 0; Wire.image[0x28] = 0x11;
    Wire.afterRequest = [&](TwoWire& w) { if (commits++ == 0) { w.image[0x28] = 0xAA; w.bumpCounter(); } };
    uint16_t captured = d.readCounter(); uint8_t b = 0; d.readBytes(0x28, &b, 1);
    printf("[readData] plain readBytes: captured counter=%u data=0x%02X counter now=%u (data belongs to the next reading)\n", captured, b, d.readCounter());
    commits = 0; Wire.image[0x28] = 0x11; d.captureReading(); unsigned tx = Wire.transactions;
    bool ok = d.readData(0x28, &b, 1);
    printf("[readData] one commit during the read: ok=%d data=0x%02X counter=%u moved=%d transactions=%u\n", ok, b, d.readCounter(), d.dataMoved(), Wire.transactions - tx);
    Wire.afterRequest = [](TwoWire& w) { w.image[0x28]++; w.bumpCounter(); };
    d.captureReading(); tx = Wire.transactions; ok = d.readData(0x28, &b, 1);
    printf("[readData] a commit on every transaction: ok=%d moved=%d transactions=%u (bounded by NW_DATA_RETRIES=%d)\n", ok, d.dataMoved(), Wire.transactions - tx, NW_DATA_RETRIES);
    Wire.afterRequest = nullptr; tx = Wire.transactions; ok = d.readData(0x28, &b, 1);
    printf("[readData] after the runaway stops: ok=%d moved=%d transactions=%u (one recapture)\n", ok, d.dataMoved(), Wire.transactions - tx);
    tx = Wire.transactions; ok = d.readData(0x28, &b, 1);
    printf("[readData] quiet device: ok=%d moved=%d transactions=%u\n", ok, d.dataMoved(), Wire.transactions - tx); }

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

  // 9. takeReadings: the N-readings loop. Five readings through a counting lambda with the batch
  //    word seen by the stub; then a chip that reports absent on the first reading stops a batch of 10.
  loadImage(); firmware(); Wire.deviceAddress = 0x41;
  { NW_Device d; d.begin(0x41, "Apis", 2); int calls = 0; lastRequest = 0;
    uint16_t taken = d.takeReadings(0x01, 5, [&] { calls++; return d.takeReading(0x01); });
    printf("[takeReadings] n=5: taken=%u calls=%d lastRequest=%u batchFaulted=%d\n", taken, calls, lastRequest, d.batchFaulted(0x01));
    onReading = [](TwoWire& w) { w.image[0x20] = 0x83; w.image[0x27] = 0x01; };   // LiDAR: no acknowledge
    calls = 0; taken = d.takeReadings(0x01, 10, [&] { calls++; return d.takeReading(0x01) && !d.faulted(0); });
    printf("[takeReadings] dead chip, n=10: taken=%u calls=%d batchFaulted=%d note=%s\n", taken, calls, d.batchFaulted(0x01), d.fault().note(nullptr, 0).c_str());
    onReading = nullptr;
    taken = d.takeReadings(0x01, 0, [&] { return true; }); printf("[takeReadings] n=0: taken=%u\n", taken); }

  // 10. NW_ReadingsConfig and nwScaled.
  { NW_ReadingsConfig c; printf("[config] defaults n=%u stats=%d columns=%d\n", c.n, c.stats, c.columns());
    printf("[config] set(5,16)=%u set(99,16)=%u set(0,16)=%u\n", c.set(5, 16), c.set(99, 16), c.set(0, 16));
    c.set(3, 16); c.stats = true; printf("[config] n=3 stats: columns=%d; n=1 stats: columns=%d\n", c.columns(), (c.set(1, 16), c.columns()));
    printf("[nwScaled] 101325/100=%.2f  -9999 passes=%.0f  mean of empty=%.0f\n", nwScaled(101325, 100.0), nwScaled(NW_ERROR, 100.0), nwScaled(NW_Readings<int16_t, 4>().mean(), 100.0)); }

  // Fault text with a library's chip-name table: chip 0, chip 1, unit, and a chip beyond the table.
  { static const char* const chips[] = {"MS5803", "MCP9808"}; NW_Fault f; char b[48];
    uint8_t codes[] = {0x01, 0x22, 0xE6, 0x51, 0x00};
    for (uint8_t code : codes) { f.code = code; BufferPrint bp(b, sizeof b); f.print(bp, chips, 2);
      printf("[fault text] code=0x%02X text='%s' note='%s'\n", code, b, f.note(chips, 2).c_str()); } }

  fprintf(stderr, "bus transactions total: %u\n", Wire.transactions);
  return 0;
}

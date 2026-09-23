// NW_Core_Demo: the Core pieces used directly, without a sensor library.
// Talks to a Schema 1 device named "Apis" at 0x41, takes five range readings
// per second as one batch, and prints their statistics and any fault.
// A real sketch uses the sensor's library (Apis, Walrus, Haar), which wraps
// exactly these calls.
#include <NW_Core.h>

NW_Device device;
NW_Readings<int16_t, 16> range;   // as served: cm
NW_ReadingsConfig rangeCfg;       // how many readings per loop; stats columns?
static const char* const chips[] = {"LiDAR", "accelerometer"};   // the spec's chip table

bool readRange() {                // one reading of chip 0 into the array
  uint8_t d[2];
  if (!device.takeReading(0x01) || !device.readBytes(NW_REG_DATA, d, 2) || device.faulted(0)) return false;
  range.append((int16_t)(d[0] | (d[1] << 8)));
  return true;
}

void setup() {
  Serial.begin(9600);
  if (!device.begin(0x41, "Apis", 2, 100)) {   // Schema 1, name, minimum firmware patch, 100 ms boot wait
    Serial.print("refused: ");
    Serial.println(device.beginFailure());
  }
  rangeCfg.set(5, range.capacity());
  Serial.println("Range mean [cm],Range std [cm],Range median [cm],Fault");
}

void loop() {
  range.reset();
  device.takeReadings(0x01, rangeCfg.n, readRange);   // batch of 5; stops early on a dead chip
  Serial.print(nwScaled(range.mean(), 1.0)); Serial.print(',');
  Serial.print(nwScaled(range.std(), 1.0));  Serial.print(',');
  Serial.print(range.median());               Serial.print(',');
  device.fault().print(Serial, chips, 2);     // "LiDAR: timeout" or "none"
  Serial.println();
  delay(1000);
}

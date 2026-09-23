# NW_Core

Shared foundation for Northern Widget sensor libraries. Unversioned and unregistered during the 2026 library overhaul; it becomes a Library Manager dependency of every NW sensor library once the overhaul is finished.

Three parts, one include:

```cpp
#include <NW_Core.h>
```

- **`NW_Device`** speaks the [NW-Device-Specification](https://github.com/NorthernWidget/NW-Device-Specification) Schema 1 register map over I2C: Page 0 identity with the three `begin()` gates (schema byte, name, minimum firmware patch) and a boot-time retry, with `beginFailure()` naming the gate that refused; the Block 0 handshake in three steps, `requestReading(chips)`, `waitReading()`, `captureReading()`, or `takeReading(chips)` for all three; batches through the readings-requested word (`beginBatch(n)`, `batchFaulted(chips)`, and `takeReadings(chips, n, readOne)`, the N-readings loop every library runs); the Report register, read once in `begin()` so boot reports survive; `readData()`, the data read checked against the counter with a bounded retry (`dataMoved()` when a device outruns it); `printSnapshot(out, chipNames, n)`, one status line for a logger's status file (name, serial, versions, the last report, Pages 0 to 2 in hex); register reads split at the 32-byte Wire buffer.
- **`NW_Readings<T, CAPACITY>`** holds one measurement's readings in a fixed array, no heap: every acquisition appends; `last()` is the scalar; `mean()`, `std()`, `sterr()`, `median()` are computed from the array on demand, so a batch logged to a file has its statistics without a second acquisition.
- **`NW_Error.h`** defines `NW_ERROR` (-9999), the missing value on file, and `nwScaled(v, divisor)`, which scales a statistic from register units and passes the sentinel through. **`NW_ReadingsConfig`** holds, per chip group, how many readings `updateMeasurements()` takes (`set(n, capacity)` clamps) and whether its std and sterr columns print (`columns()`).
- **`NW_Report`** decodes the status and Report bytes and prints the whole report (`print(out, chipNames, n)`: "MS5803: no acknowledge") or gives it as one word for a note column (`note(chipNames, n)`: "MS5803NoACK"), the library passing its chip-name table; `printKind()` and `kindWord()` give the universal kind alone.

A sensor library holds one `NW_Device` and one `NW_Readings` per measurement and forwards to them; nothing inherits. The design, and the line between what lives here and what stays in each library, is in [LIBRARY-DESIGN.md](https://github.com/NorthernWidget/NW-Device-Specification/blob/master/LIBRARY-DESIGN.md), section 11.

## Using it in a library

The shape every NW sensor library takes on Core (Walrus, two chip groups):

```cpp
class Walrus {
    NW_Device _dev;
    NW_Readings<int32_t, WALRUS_PRESSURE_CAPACITY> _pressureReadings;   // uBar, as served
    NW_ReadingsConfig _pressureCfg;                                    // how many readings; stats columns?
  public:
    enum Component : uint8_t { MS5803 = 0x01, MCP9808 = 0x02, ALL = 0x03 };   // the chip-select bits
    bool begin(uint8_t address = DEFAULT_ADDRESS) { return _dev.begin(address, "Walrus", WALRUS_FW_MIN_PATCH); }
    bool updatePressure() {                                            // one reading of one chip group
        uint8_t d[6];
        if (!_dev.takeReading(MS5803) || !_dev.readData(PRES_REG, d, 6) || _dev.faulted(0)) return false;
        _pressureReadings.append((int32_t)(d[0] | (d[1] << 8) | ((uint32_t)d[2] << 16) | ((uint32_t)d[3] << 24)));
        return true;
    }
    bool updateMeasurements(uint8_t component = ALL) {                 // N readings, then the means
        _pressureReadings.reset();
        _dev.takeReadings(MS5803, _pressureCfg.n, [this] { return updatePressure(); });
        _pressure = nwScaled(_pressureReadings.mean(), 1000.0);        // mBar; NW_ERROR when none
        return _pressureReadings.count() > 0;
    }
    uint16_t setPressureReadings(uint16_t n) { return _pressureCfg.set(n, WALRUS_PRESSURE_CAPACITY); }
    float getPressureStd() { return nwScaled(_pressureReadings.std(), 1000.0); }
    size_t printReport(Print& out) {
        static const char* const chips[] = {"MS5803", "MCP9808"};      // the spec's chip table
        return _dev.report().print(out, chips, 2);
    }
};
```

`takeReadings()` declares the batch to the device (`beginBatch`), calls the one-reading function n times, and stops once a selected chip reports absent, so a dead chip costs one reading. `getHeader()`/`getString()` print std and sterr columns when `_pressureCfg.columns()` is true. The full pattern, with the reading interface (`beginReadings`, `printHeader`, `printReading`, `logReading`, `endReadings`), is in Apis_Library, Walrus_Library and Haar_Library.

## Testing

`extras/test/run.sh` compiles the library on a desktop against stub `Arduino.h` and `Wire.h` (an emulated Schema 1 device with firmware behaviour, boot delay, free running, faults, and the 32-byte Wire buffer) and checks that the output is byte-identical to `extras/test/baseline.txt`. The stubs are shared: every NW library's harness includes them from here (`NW_CORE` path, default the sibling checkout). `--record` rewrites the baseline when a change is intended.

**Full API reference:** https://docs.northernwidget.com/NW_Core/

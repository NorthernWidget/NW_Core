# NW_Core

Shared foundation for Northern Widget sensor libraries. Unversioned and unregistered during the 2026 library overhaul; it becomes a Library Manager dependency of every NW sensor library once the overhaul is finished.

Three parts, one include:

```cpp
#include <NW_Core.h>
```

- **`NW_Device`** speaks the [NW-Device-Specification](https://github.com/NorthernWidget/NW-Device-Specification) Schema 1 register map over I2C: Page 0 identity with the three `begin()` gates (schema byte, name, minimum firmware patch) and a boot-time retry, with `beginFailure()` naming the gate that refused; the Block 0 handshake in three steps, `requestReading(chips)`, `waitReading()`, `captureReading()`, or `takeReading(chips)` for all three; batches through the readings-requested word (`beginBatch(n)`, `batchFaulted(chips)`, and `takeReadings(chips, n, readOne)`, the N-readings loop every library runs); the latched fault; register reads split at the 32-byte Wire buffer.
- **`NW_Readings<T, CAPACITY>`** holds one measurement's readings in a fixed array, no heap: every acquisition appends; `last()` is the scalar; `mean()`, `std()`, `sterr()`, `median()` are computed from the array on demand, so a batch logged to a file has its statistics without a second acquisition.
- **`NW_Error.h`** defines `NW_ERROR` (-9999), the missing value on file, and `nwScaled(v, divisor)`, which scales a statistic from register units and passes the sentinel through. **`NW_ReadingsConfig`** holds, per chip group, how many readings `updateMeasurements()` takes (`set(n, capacity)` clamps) and whether its std and sterr columns print (`columns()`).
- **`NW_Fault`** decodes the status and latched-fault bytes and gives the universal fault kind in words (`printKind()`) or as one word for a note column (`kindWord()`); the library prints its own chip name.

A sensor library holds one `NW_Device` and one `NW_Readings` per measurement and forwards to them; nothing inherits. The design, and the line between what lives here and what stays in each library, is in [LIBRARY-DESIGN.md](https://github.com/NorthernWidget/NW-Device-Specification/blob/master/LIBRARY-DESIGN.md), section 11.

## Using it in a library

```cpp
class Apis {
    NW_Device _dev;
    NW_Readings<int16_t, 64> _range;
  public:
    bool begin(uint8_t address = DEFAULT_ADDRESS) { return _dev.begin(address, "Apis", 2, 100); }
    bool updateRange() {
        if (_dev.batchFaulted() || !_dev.takeReading(0x01)) return false;   // chip 0
        uint8_t d[2]; _dev.readBytes(NW_REG_DATA, d, 2);
        _range.append((int16_t)((d[1] << 8) | d[0]));
        return true;
    }
    float getRangeMean() { return _range.mean(); }
    size_t printFault(Print& out) {
        size_t n = out.print(_dev.faultChip() == 0 ? "LiDAR" : "accelerometer");
        n += out.print(": ");
        return n + _dev.fault().printKind(out);
    }
};
```

## Testing

`extras/test/run.sh` compiles the library on a desktop against stub `Arduino.h` and `Wire.h` (an emulated Schema 1 device with firmware behaviour, boot delay, free running, faults, and the 32-byte Wire buffer) and checks that the output is byte-identical to `extras/test/baseline.txt`. The stubs are shared: every NW library's harness includes them from here (`NW_CORE` path, default the sibling checkout). `--record` rewrites the baseline when a change is intended.

**Full API reference:** https://docs.northernwidget.com/NW_Core/

/**
 * @file NW_Device.h
 * @brief The NW-Device-Specification Schema 1 device protocol over I2C.
 */
#ifndef NW_Device_h
#define NW_Device_h

#include <Arduino.h>
#include <Wire.h>
#include "NW_Fault.h"

// Page 0 (identity, served from EEPROM) and Page 1 Block 0 (status and control),
// as NW-Device-Specification defines them for every device.
#define NW_REG_SCHEMA    0x00  ///< 0x01 = Schema 1
#define NW_REG_NAME      0x01  ///< 7-byte device name, null-padded (0x01-0x07)
#define NW_REG_HW_MAJOR  0x08
#define NW_REG_HW_MINOR  0x09
#define NW_REG_FW_PATCH  0x0A  ///< written by the firmware
#define NW_REG_I2C_ADDR  0x1F  ///< writable; takes effect at the device's next boot
#define NW_REG_STATUS    0x20  ///< bit 0 ready; bits 1-6 chip faults; bit 7 pan-fault
#define NW_REG_CTRL      0x21  ///< writable: bit 0 trigger; bits 1-6 chip select; bit 7 sleep
#define NW_REG_COUNTER   0x22  ///< reading counter, uint16 little-endian (0x22-0x23)
#define NW_REG_REQUEST   0x24  ///< readings requested (batch size), uint16 little-endian, writable (0x24-0x25)
#define NW_REG_CONFIG    0x26  ///< writable, device-specific, volatile
#define NW_REG_FAULT     0x27  ///< latched fault code; cleared by any Control write
#define NW_REG_DATA      0x28  ///< first device data byte
#define NW_BIT_READY     0x01
#define NW_BIT_PANFAULT  0x80
#define NW_CTRL_TRIGGER  0x01
#define NW_CTRL_SLEEP    0x80
#define NW_WIRE_CHUNK    32    ///< the AVR Wire buffer; longer reads are split into transactions of this size

/**
 * @brief One NW Schema 1 device on the I2C bus: identity gates, the reading handshake, batches, faults.
 * @details A sensor library holds one of these and forwards to it. The
 * handshake is three steps so that a library can trigger a reading and come
 * back for it later: requestReading(chips), waitReading(), captureReading();
 * takeReading(chips) does all three. Readings started by the device itself
 * (a free-running device) are seen through the counter: newReading() and
 * waitReading() work without a request.
 */
class NW_Device {
  public:
    /**
     * @brief Open the device and check that it is what the library expects.
     * @details Starts Wire, waits for an acknowledge at the address for up to
     * bootTimeoutMs (a logger often calls begin() the instant it powers the
     * sensor rail), reads Page 0 bytes 0x00-0x0F, stores the hardware and
     * firmware versions, then refuses the device unless the schema byte is
     * 0x01, the 7-byte name matches, and the firmware patch is at least
     * minPatch. The versions are stored before any refusal so a sketch can
     * report why.
     * @param address       7-bit I2C address
     * @param name          the device's name as the spec spells it (up to 7 characters)
     * @param minPatch      lowest firmware patch (Page 0 byte 0x0A) this library accepts
     * @param bootTimeoutMs how long to keep retrying the first acknowledge (0 = one try)
     * @return true if the device answered and passed the three gates
     */
    bool begin(uint8_t address, const char* name, uint8_t minPatch, unsigned long bootTimeoutMs = 0);
    /**
     * @brief Why the last begin() refused, as one word for a data-table note:
     * "NoACK", "ReadFailed", "NotSchema1", "WrongName", "OldFirmware"; "None"
     * after a successful begin().
     */
    String beginFailure() const;
    uint8_t address() const          { return _adr; }
    uint8_t hardwareMajor() const    { return _hwMajor; }
    uint8_t hardwareMinor() const    { return _hwMinor; }
    uint8_t firmwareVersion() const  { return _fwPatch; }

    /** @brief Write a new I2C address to Page 0 (0x1F); the device uses it from its next boot. */
    bool setI2CAddress(uint8_t newAddress);
    /**
     * @brief Ceiling on the wait for a reading [ms]. Not a delay: waitReading() returns as
     * soon as the counter moves. Must exceed the device's slowest path to ready, which is
     * its fault path (see the device appendix); default 500.
     */
    void setTimeout(unsigned long ms) { _timeout = ms; }
    unsigned long timeout() const     { return _timeout; }

    // --- Handshake ---
    /** @brief Status bit 0: the data registers hold a complete reading. */
    bool ready();
    /** @brief The reading counter (0x22-0x23). */
    uint16_t readCounter();
    /** @brief The counter differs from the last captured reading's (true before any capture). */
    bool newReading();
    /**
     * @brief Trigger a reading of the given chips.
     * @param chips bit n = chip n (0..5); the control byte gets trigger | chips << 1
     */
    bool requestReading(uint8_t chips);
    /** @brief Wait until the counter moves past the value seen at the last request (or the last capture), within timeout(). */
    bool waitReading();
    /** @brief Read Block 0 after a reading: status and latched fault into fault(); notes the counter. */
    bool captureReading();
    /** @brief requestReading() + waitReading() + captureReading(). false on bus error or timeout. */
    bool takeReading(uint8_t chips);

    // --- Batches (NW-Device-Specification 0x24-0x25) ---
    /**
     * @brief Declare how many readings follow, so the device holds its chips powered for exactly that many.
     * @details Also clears batchFaulted(). 0 or 1 means one reading per trigger, powered down after each.
     */
    bool writeBatch(uint16_t n);
    /** @brief Forget a previous batch's absent chips without writing the size (single readings). */
    void resetBatch()                 { _absentChips = 0; }
    /**
     * @brief Start a batch of n readings: writeBatch(n) when n > 1, else resetBatch().
     * @details The library's loop then calls takeReading() n times, appending
     * each successful reading to its NW_Readings, and stops early on
     * batchFaulted(chips). One reading is a batch of one (n = 0 or 1), which
     * writes nothing to the device.
     * @return false if the batch word could not be written
     */
    bool beginBatch(uint16_t n)       { if (n > 1) return writeBatch(n); resetBatch(); return true; }
    /**
     * @brief Take up to n readings of the given chips, one at a time, through readOne().
     * @details beginBatch(n), then readOne() n times; stops early once a selected
     * chip has reported absent (batchFaulted(chips)), so a dead chip costs one
     * reading, not n. readOne is any callable returning bool: true when it stored
     * a reading (a library's updateRange(), updatePressure(), ...).
     * @return how many calls to readOne() returned true
     */
    template <typename F>
    uint16_t takeReadings(uint8_t chips, uint16_t n, F readOne) {
      beginBatch(n);
      uint16_t taken = 0;
      for (uint16_t i = 0; i < n; i++) {
        if (readOne()) taken++;
        else if (batchFaulted(chips)) break;
      }
      return taken;
    }
    /**
     * @brief Any of the given chips reported, earlier in this batch, that it is not coming
     * (no acknowledge or not initialised). Per chip: an absent accelerometer does not stop
     * the range readings of the same batch.
     * @param chips bit n = chip n; default: any chip
     * @details Set by captureReading() for a selected chip; a library skips that chip's
     * remaining readings instead of waiting out each one.
     */
    bool batchFaulted(uint8_t chips = 0x3F) const { return (_absentChips & chips) != 0; }

    // --- Faults ---
    const NW_Fault& fault() const     { return _fault; }
    bool faulted(uint8_t chip) const  { return _fault.chipFaulted(chip); }
    bool anyFault() const             { return _fault.any(); }
    uint8_t faultChip() const         { return _fault.chip(); }
    uint8_t faultKind() const         { return _fault.kind(); }

    // --- Registers ---
    /** @brief Read n bytes from reg; reads longer than NW_WIRE_CHUNK are split into several transactions. */
    bool readBytes(uint8_t reg, uint8_t* buf, uint8_t n);
    bool writeByte(uint8_t reg, uint8_t value);
    uint8_t readConfig();
    bool writeConfig(uint8_t value)   { return writeByte(NW_REG_CONFIG, value); }
    /** @brief Ask the device to enter its lowest-power state (Control bit 7); it wakes on its next address match. */
    bool sleep()                      { return writeByte(NW_REG_CTRL, NW_CTRL_SLEEP); }

  private:
    uint8_t _adr = 0;
    uint8_t _hwMajor = 0, _hwMinor = 0, _fwPatch = 0;
    uint8_t _beginFailure = 0;          // 0 none, 1 no ACK, 2 read failed, 3 schema, 4 name, 5 firmware
    unsigned long _timeout = 500;
    uint16_t _lastCounter = 0xFFFF;     // counter of the last captured reading
    uint16_t _counterBefore = 0xFFFF;   // counter seen at the last request
    uint8_t  _chips = 0;                // chips selected at the last request
    uint8_t _absentChips = 0;           // chips that reported absent since the last writeBatch()/resetBatch()
    NW_Fault _fault;
};

#endif

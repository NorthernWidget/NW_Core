/**
 * @file NW_Fault.h
 * @brief Decoded Block 0 status (0x20) and latched fault (0x27) of a Schema 1 device.
 */
#ifndef NW_Fault_h
#define NW_Fault_h

#include <Arduino.h>

/**
 * @brief The two Block 0 bytes that describe a reading's health, with their decoding.
 * @details status (0x20) is live: bit 0 ready, bits 1-6 chip n-1 faulted now,
 * bit 7 pan-fault (OR of bits 1-6). code (0x27) is latched until the controller
 * next writes Control: bits 7-5 chip (7 = the unit), bits 4-0 kind.
 * Kinds (NW-Device-Specification): 0 none, 1 no acknowledge, 2 timeout,
 * 3 checksum, 4 out of range, 5 not initialised, 6 reset since configured,
 * 7 config rejected, 8 supply fault, 9-15 reserved, 16-31 device-specific.
 * The chip names are the device's own; a library prints them itself and calls
 * printKind() for the universal part.
 */
struct NW_Fault {
    uint8_t status = 0;   ///< Block 0 byte 0x20
    uint8_t code   = 0;   ///< Block 0 byte 0x27

    bool ready() const                    { return status & 0x01; }
    bool chipFaulted(uint8_t chip) const  { return status & (1 << (chip + 1)); }   ///< chip 0..5 faulted on the last reading
    bool any() const                      { return status & 0x80; }               ///< pan-fault
    uint8_t chip() const                  { return code >> 5; }                   ///< 0..6, or 7 for the unit
    uint8_t kind() const                  { return code & 0x1F; }
    bool isUnit() const                   { return chip() == 7; }

    /** @brief Print the kind in words ("timeout"; "kind 17" for device-specific kinds). */
    size_t printKind(Print& out) const {
        switch (kind()) {
            case 0: return out.print(F("none"));
            case 1: return out.print(F("no acknowledge"));
            case 2: return out.print(F("timeout"));
            case 3: return out.print(F("checksum"));
            case 4: return out.print(F("out of range"));
            case 5: return out.print(F("not initialised"));
            case 6: return out.print(F("reset since configured"));
            case 7: return out.print(F("config rejected"));
            case 8: return out.print(F("supply fault"));
            default: { size_t n = out.print(F("kind ")); return n + out.print(kind()); }
        }
    }
    /** @brief True for the kinds that mean the chip is not coming back this batch (no acknowledge, not initialised). */
    bool chipAbsent() const               { return kind() == 1 || kind() == 5; }
};

#endif

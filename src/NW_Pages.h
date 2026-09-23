/**
 * @file NW_Pages.h
 * @brief The served pages of a device that runs on a logger: Margay's and Okapi's reading of themselves.
 */
#ifndef NW_Pages_h
#define NW_Pages_h

#include <Arduino.h>
#include <EEPROM.h>
#include "NW_Report.h"

/**
 * @brief The register image a Schema 1 device keeps: Pages 0-1 from EEPROM, Pages 2-3 written per reading.
 * @details The counterpart of NW_Device. A logger library holds one, loads the stored
 * half at boot (loadStored), fills Page 2 at each reading between beginReading() and
 * endReading(), and latches its reports into Block 0 with the spec's rules: a fault
 * overwrites a notice, a notice never overwrites an unacknowledged fault, and the
 * counter moves when the reading is complete. printSnapshot() is the status line a
 * logger writes for itself, in the same columns as NW_Device::printSnapshot(). Nothing
 * here serves a bus: a transport (UART, Okapi) reads page[] when it comes.
 */
class NW_Pages {
  public:
    static const uint8_t SIZE = 128;      ///< Pages 0-3, 0x00-0x7F
    uint8_t page[SIZE];

    NW_Pages() { memset(page, 0, SIZE); }

    /** @brief Pages 0 and 1 from EEPROM[base .. base + 63] (base = EEPROM.length() - 64). */
    void loadStored(int base) { for (uint8_t i = 0; i < 64; i++) page[i] = EEPROM.read(base + i); }
    /** @brief Page 0 says Schema 1, carries the magic byte and passes its CRC. */
    bool page0Valid() const { return page[0] == 0x01 && page[0x1D] == 0x4E && crc8(page, 0x1E) == page[0x1E]; }
    /** @brief Page 1 was never written (every byte 0xFF): use built-in constants instead. */
    bool page1Blank() const { for (uint8_t i = 0x20; i < 0x40; i++) if (page[i] != 0xFF) return false; return true; }

    // --- Block 0 (0x40-0x47) ---
    /** @brief A reading begins: ready clears. Fill Page 2 after this. */
    void beginReading() { page[0x40] &= ~0x01; }
    /**
     * @brief A reading is complete: status carries the chip-fault bits given (bit n = chip n),
     * the pan-fault bit follows them, the counter moves, ready sets.
     */
    void endReading(uint8_t chipFaults) {
      uint8_t status = 0x01 | (uint8_t)((chipFaults & 0x3F) << 1);
      if (status & 0x7E) status |= 0x80;
      uint16_t c = counter() + 1;
      page[0x42] = c & 0xFF; page[0x43] = c >> 8;
      page[0x40] = status;
    }
    uint16_t counter() const { return page[0x42] | (page[0x43] << 8); }
    /** @brief Latch a fault code (chip in bits 7-5, kind in bits 4-0): overwrites anything. */
    void latchFault(uint8_t code) { page[0x47] = code; }
    /** @brief Latch a notice unless a fault is waiting to be acknowledged (spec: one report at a time). */
    void latchNotice(uint8_t code) { if (!isFaultCode(page[0x47])) page[0x47] = code; }
    /** @brief The controller's acknowledge: a logger acknowledges its own report once it has written the row. */
    void acknowledge() { page[0x47] = 0; }
    /** @brief The report as NW_Report decodes it: status byte and code. */
    NW_Report report() const { NW_Report r; r.status = page[0x40]; r.code = page[0x47]; return r; }

    // --- Data helpers (little-endian, bus addresses) ---
    void put8(uint8_t at, uint8_t v)   { page[at] = v; }
    void put16(uint8_t at, uint16_t v) { page[at] = v & 0xFF; page[at + 1] = v >> 8; }
    void put32(uint8_t at, uint32_t v) { for (uint8_t i = 0; i < 4; i++) page[at + i] = (v >> (8 * i)) & 0xFF; }
    uint16_t get16(uint8_t at) const   { return page[at] | (page[at + 1] << 8); }
    float getFloat(uint8_t at) const   { float f; memcpy(&f, page + at, 4); return f; }

    /**
     * @brief One status line for the logger's status file, the columns of NW_Device::printSnapshot():
     * name, serial (Page 0 Block 2), HW major.minor, FW (the library version, given), FWCommit (its
     * build commit, given), Lib and LibCommit (blank and the sketch's commit for a logger), report code and
     * note word (r, the current report by default; device-specific kinds through kindWords), Pages 0, 1 and 2 in hex. No newline.
     */
    size_t printSnapshot(Print& out, const char* const* chipNames, uint8_t nChips, const char* fw, const NW_Report* r = nullptr,
                         const char* const* kindWords = nullptr, uint8_t nKindWords = 0,
                         const char* fwCommit = "", const char* lib = "", const char* libCommit = "") const {
      NW_Report cur = report();
      if (!r) r = &cur;
      size_t n = 0;
      for (uint8_t i = 1; i <= 7 && page[i]; i++) n += out.print((char)page[i]);
      n += out.print(',');
      for (uint8_t i = 0; i < 4; i++) { if (i) n += out.print('-'); n += nwPrintHex(out, page + 0x10 + 2 * i, 2); }
      n += out.print(','); n += out.print(page[0x08]); n += out.print('.'); n += out.print(page[0x09]);
      n += out.print(','); n += out.print(fw);
      n += out.print(','); n += out.print(fwCommit);                                   // the logger's library commit: its firmware's
      n += out.print(','); n += out.print(lib); n += out.print(','); n += out.print(libCommit);   // for a logger: blank, and the sketch's commit
      n += out.print(F(",0x")); n += nwPrintHex(out, &r->code, 1);
      n += out.print(','); n += out.print(r->note(chipNames, nChips, kindWords, nKindWords));
      for (uint8_t p = 0; p < 0x60; p += 0x20) { n += out.print(','); n += nwPrintPage(out, page + p); }
      return n;
    }

    /** @brief CRC-8/SMBUS, as Page 0 carries it. */
    static uint8_t crc8(const uint8_t* d, uint8_t len) {
      uint8_t c = 0;
      for (uint8_t i = 0; i < len; i++) { c ^= d[i]; for (uint8_t b = 0; b < 8; b++) c = (c & 0x80) ? (uint8_t)((c << 1) ^ 0x07) : (uint8_t)(c << 1); }
      return c;
    }
    /** @brief A report code whose kind is a fault (1-5, 7, 8), as opposed to a notice (6, 9, 10, device-specific notices). */
    static bool isFaultCode(uint8_t code) { uint8_t k = code & 0x1F; return k >= 1 && k <= 8 && k != 6; }
};

#endif

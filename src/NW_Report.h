/**
 * @file NW_Report.h
 * @brief Decoded Block 0 status (0x40) and report (0x47) of a Schema 1 device.
 */
#ifndef NW_Report_h
#define NW_Report_h

#include <Arduino.h>

/**
 * @brief The two Block 0 bytes that describe a reading's health, with their decoding.
 * @details status (0x40) is live: bit 0 ready, bits 1-6 chip n-1 faulted now,
 * bit 7 pan-fault (OR of bits 1-6). code (0x47) is the Report register, the
 * device's most recent report, good or bad, latched until the controller next
 * writes Control: bits 7-5 chip (7 = the unit), bits 4-0 kind. A report is a
 * fault when the chip's status bit is set too, a notice otherwise.
 * Kinds (NW-Device-Specification): 0 none, 1 no acknowledge, 2 timeout,
 * 3 checksum, 4 out of range, 5 not initialised, 6 reset since configured
 * (a notice), 7 config rejected, 8 supply fault, 9 calibration stored (a
 * notice), 10 batch abandoned (a notice), 11-15 reserved, 16-31 device-specific.
 * The chip names are the device's own: a library passes its table (the spec's
 * chip table, in order) to print() and note(); printKind() and kindWord() give
 * the universal part alone.
 */
struct NW_Report {
  uint8_t status = 0;   ///< Block 0 byte 0x40
  uint8_t code   = 0;   ///< Block 0 byte 0x47

  bool ready() const                    { return status & 0x01; }
  bool chipFaulted(uint8_t chip) const  { return status & (1 << (chip + 1)); }   ///< chip 0..5 faulted on the last reading
  bool any() const                      { return status & 0x80; }               ///< pan-fault
  uint8_t chip() const                  { return code >> 5; }                   ///< 0..6, or 7 for the unit
  uint8_t kind() const                  { return code & 0x1F; }
  bool isUnit() const                   { return chip() == 7; }
  /** @brief The report is a fault: its chip's status bit is set (the unit: pan-fault), so the data are not to be trusted. */
  bool isFault() const                  { return kind() != 0 && (isUnit() ? any() : chipFaulted(chip())); }
  /** @brief The report is a notice: a code with no status bit behind it (reset, calibration stored, batch abandoned). */
  bool isNotice() const                 { return kind() != 0 && !isFault(); }

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
      case 9: return out.print(F("calibration stored"));
      case 10: return out.print(F("batch abandoned"));
      default: { size_t n = out.print(F("kind ")); return n + out.print(kind()); }
    }
  }
  /** @brief The kind as one word for a data-table note ("Timeout"; "Kind17" for device-specific kinds). */
  String kindWord() const {
    switch (kind()) {
      case 0: return String(F("None"));
      case 1: return String(F("NoACK"));
      case 2: return String(F("Timeout"));
      case 3: return String(F("Checksum"));
      case 4: return String(F("Range"));
      case 5: return String(F("NotInit"));
      case 6: return String(F("Reset"));
      case 7: return String(F("Config"));
      case 8: return String(F("Supply"));
      case 9: return String(F("Calibrated"));
      case 10: return String(F("BatchAbandoned"));
      default: { String w = F("Kind"); w += String(kind()); return w; }
    }
  }
  /**
   * @brief Print the report in words: the chip, then the kind, e.g.
   * "MS5803: no acknowledge", "unit: reset since configured"; "none" when
   * there is no report. A library passes its chip-name table from the spec's
   * chip table; a chip beyond it prints as "chip N".
   * @return Bytes written.
   */
  size_t print(Print& out, const char* const* chipNames, uint8_t nChips) const {
    uint8_t c = chip(), k = kind();
    if (k == 0) return out.print(F("none"));
    size_t n = 0;
    if (c == 7) n += out.print(F("unit"));
    else if (c < nChips) n += out.print(chipNames[c]);
    else { n += out.print(F("chip ")); n += out.print(c); }
    n += out.print(F(": "));
    return n + printKind(out);
  }
  /**
   * @brief The report as one word for a data-table note column: the
   * chip, then the kind, e.g. "MS5803NoACK", "UnitReset", "Chip2Kind17";
   * "UnitNone" when there is no report.
   */
  String note(const char* const* chipNames, uint8_t nChips) const {
    uint8_t c = chip();
    String w;
    if (kind() == 0) return String(F("UnitNone"));
    if (c == 7) w = F("Unit");
    else if (c < nChips) w = chipNames[c];
    else { w = F("Chip"); w += String(c); }
    w += kindWord();
    return w;
  }
  /** @brief True for the kinds that mean the chip is not coming back this batch (no acknowledge, not initialised). */
  bool chipAbsent() const               { return kind() == 1 || kind() == 5; }
};

#endif

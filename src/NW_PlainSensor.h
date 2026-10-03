/*
NW_PlainSensor: a sensor a logger can hold that has no identity page.
Licensed: GNU GPL v3
*/

#ifndef NW_PlainSensor_h
#define NW_PlainSensor_h

#include "NW_Report.h"
#include "NW_Sensor.h"

/**
 * @brief What a logger can hold that is not a Schema 1 device.
 * @details A third-party chip has no Page 0 and no Report register, so it cannot
 * name itself on the bus and cannot be found by NW_Logger::discover(): a device
 * with no valid Page 0 never matches a candidate name, which is why discovery
 * needs no special case for one of these (Andy, 2026-10-03: a non-NW sensor has
 * no schema and therefore cannot be automatically detected). A sketch can still
 * name it, and watch() then logs its columns like any other sensor's.
 *
 * This fills in the half of NW_Sensor that a device with no Report register
 * cannot answer, so a library supplies only what it genuinely knows: name(),
 * defaultAddress(), wake(), acquire(), printDataHeader() and printDataRow().
 *
 * The report is the LIBRARY's rather than the device's, which is the honest
 * reading of it: latchFault(NW_PLAIN_NOT_ANSWERING) when the chip does not
 * acknowledge, latchFault(NW_PLAIN_TIMEOUT) when a reading does not arrive. The
 * kinds are the specification's universal ones, so the note word a logger writes
 * reads the same as any other sensor's: "T9602NotAnswering", "T9602Timeout".
 *
 * Its status row carries the same columns as a Schema 1 sensor's and writes a
 * lone `-` in each one a Page 0 would have filled: serial, hardware version,
 * firmware patch, firmware commit and the three pages. A row with a device name
 * and a dash where its serial belongs says "this device has no identity page",
 * which is a fact about the device rather than a field that failed to read.
 */
class NW_PlainSensor : public NW_Sensor {
public:
  /** @brief Kind of the report this library latched during the last reading (0 = none). */
  uint8_t reportKind() override {
    return _report.kind();
  }

  bool reportIsFault() override {
    return _report.isFault();
  }

  /**
   * @brief Always 0: a chip with no Report register cannot say it restarted.
   * @details A Schema 1 device latches a report at its own power-up and serves
   * it until a controller acknowledges. There is nowhere for this one to keep
   * such a thing, and inventing one would put a word in the status file that no
   * device said.
   */
  uint8_t bootReportKind() override {
    return 0;
  }

  void clearBootReport() override {}

  /**
   * @brief One status row, with a lone `-` wherever a Page 0 would have answered.
   * @details Columns, as NW_Pages::printSnapshot() writes them: name, serial,
   * hardware version, firmware patch, firmware commit, library version, library
   * commit, report code, note word, then Pages 0, 1 and 2. This device has none
   * of the first group and no pages, so each is a dash.
   * @param boot ignored: there is no boot report to tell apart from the current
   *        one. The parameter stays because the logger calls every sensor the
   *        same way.
   */
  size_t printStatus(Print& out, bool boot = false) override {
    (void)boot;
    const char* chips[] = { name() };
    size_t n = out.print(name());
    for (uint8_t i = 0; i < 4; i++) n += out.print(F(",-"));  // serial, HW, FW, FW commit
    n += out.print(',');
    n += out.print(_lib);
    n += out.print(',');
    n += out.print(_libCommit);
    n += out.print(F(",0x"));
    n += nwPrintHex(out, &_report.code, 1);
    n += out.print(',');
    n += _report.printNote(out, chips, 1);
    for (uint8_t i = 0; i < 3; i++) n += out.print(F(",-"));  // Pages 0, 1 and 2
    return n;
  }

  /**
   * @brief One word for the logger's Note column, with no comma.
   * @details The device name then the kind, as any sensor's does:
   * "T9602Timeout". A failed begin() can only mean one thing here, because the
   * only gate a chip without an identity page has is whether it acknowledges.
   */
  size_t printNote(Print& out, bool beginFailed = false) override {
    if (beginFailed) return out.print(F("NotAnswering"));
    const char* chips[] = { name() };
    return _report.printNote(out, chips, 1);
  }

protected:
  /**
   * @param lib This library's version, for the status row's Lib column.
   * @param libCommit Its build commit, which the NW-Build wrapper sets and an
   *        Arduino IDE build leaves blank.
   */
  NW_PlainSensor(const char* lib, const char* libCommit = "")
    : _lib(lib), _libCommit(libCommit) {}

  /**
   * @brief Latch a fault: the data of this reading are not to be trusted.
   * @param kind A universal kind from the specification's table (1 not
   *        answering, 2 timed out, 3 checksum failed, 4 out of range).
   */
  void latchFault(uint8_t kind) {
    _report.code = (uint8_t)(kind & 0x1F);  // chip 0: the device is its own chip
    _report.status |= 0x82;                 // its chip bit, and the pan-fault bit
  }

  /** @brief Latch a notice, unless a fault is already waiting: one report at a time, as the spec says. */
  void latchNotice(uint8_t kind) {
    if (_report.isFault()) return;
    _report.code = (uint8_t)(kind & 0x1F);
    _report.status = 0;
  }

  /** @brief Nothing to report: call it at the start of a reading that goes well. */
  void clearReport() {
    _report.code = 0;
    _report.status = 0;
  }

  /** @brief The report as it stands, for a library that wants to read its own. */
  const NW_Report& report() const {
    return _report;
  }

private:
  NW_Report _report;
  const char* _lib;
  const char* _libCommit;
};

/// @name Universal report kinds a plain sensor can honestly latch
/// @{
#define NW_PLAIN_NOT_ANSWERING 1  ///< the chip did not acknowledge its address
#define NW_PLAIN_TIMEOUT 2        ///< a reading was asked for and did not arrive
#define NW_PLAIN_CHECKSUM 3       ///< the chip answered and the data did not check
#define NW_PLAIN_OUT_OF_RANGE 4   ///< the value is outside what the part can mean
/// @}

#endif

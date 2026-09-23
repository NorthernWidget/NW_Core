/**
 * @file NW_Sensor.h
 * @brief The view a logger has of any Northern Widget sensor library: name, reports, status line.
 */
#ifndef NW_Sensor_h
#define NW_Sensor_h

#include <Arduino.h>

/**
 * @brief What a data logger needs from a sensor library to keep a status file.
 * @details Every library on NW_Core (Apis, Walrus, Haar, Libelle) inherits this
 * and the logger keeps a list of them (Margay::watch()). The logger writes a
 * row for a device when its report says something happened: after a reading,
 * reportKind() is the reading's report; bootReportKind() is what begin()
 * captured before the first trigger cleared it, kept until the logger has
 * looked (clearBootReport()). A reset (kind 6) seen at boot is expected on a
 * logger that powers its sensors per reading and makes no row; any other boot
 * report, and any report after a reading, does.
 */
class NW_Sensor {
  public:
    virtual ~NW_Sensor() {}
    /** @brief The device name as the spec spells it ("Apis"). */
    virtual const char* name() const = 0;
    /** @brief Kind of the report captured with the last reading (0 = none). */
    virtual uint8_t reportKind() = 0;
    /** @brief The last report is a fault: its chip's status bit is set, the data are not to be trusted. */
    virtual bool reportIsFault() = 0;
    /** @brief Kind of the report begin() captured at the device's boot; 0 once cleared or if none. */
    virtual uint8_t bootReportKind() = 0;
    /** @brief The logger has recorded (or dismissed) the boot report. */
    virtual void clearBootReport() = 0;
    /**
     * @brief Print one status line for the logger's status file (no newline):
     * name, serial, HW, FW, report code, note word, Pages 0-2 in hex.
     * @param boot print the boot report (the code begin() captured) instead of the last reading's
     */
    virtual size_t printStatus(Print& out, bool boot = false) = 0;
};

#endif

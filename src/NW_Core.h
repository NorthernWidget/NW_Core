/**
 * @file NW_Core.h
 * @brief Northern Widget shared library foundation.
 * @details Include this to get the three parts:
 *   - NW_Device: the NW-Device-Specification Schema 1 device protocol over I2C
 *     (Page 0 identity and version gates; Block 0 trigger, ready, counter,
 *     batch, and report handling; chunked register reads).
 *   - NW_Readings<T, CAPACITY>: a measurement's readings in a fixed array
 *     (no heap) with mean, standard deviation, standard error, and median.
 *   - NW_Report: the decoded Status and Report bytes (faults and notices).
 *   - NW_Error.h: NW_ERROR (-9999), the missing value on file, and nwScaled().
 *   - NW_ReadingsConfig: how many readings a chip group takes and whether its
 *     statistics columns print.
 * Sensor libraries hold an NW_Device and one NW_Readings per measurement.
 * Names exported by this library carry the NW_ prefix; everything else in an
 * NW sensor library is scoped to its own class.
 */
#ifndef NW_Core_h
#define NW_Core_h

#include "NW_Error.h"
#include "NW_Report.h"
#include "NW_Readings.h"
#include "NW_Device.h"

#endif

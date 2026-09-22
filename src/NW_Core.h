/**
 * @file NW_Core.h
 * @brief Northern Widget shared library foundation.
 * @details Include this to get the three parts:
 *   - NW_Device: the NW-Device-Specification Schema 1 device protocol over I2C
 *     (Page 0 identity and version gates; Block 0 trigger, ready, counter,
 *     batch, and fault handling; chunked register reads).
 *   - NW_Readings<T, CAPACITY>: a measurement's readings in a fixed array
 *     (no heap) with mean, standard deviation, standard error, and median.
 *   - NW_Fault: the decoded status and latched-fault bytes.
 * Sensor libraries hold an NW_Device and one NW_Readings per measurement.
 * Names exported by this library carry the NW_ prefix; everything else in an
 * NW sensor library is scoped to its own class.
 */
#ifndef NW_Core_h
#define NW_Core_h

#include "NW_Fault.h"
#include "NW_Readings.h"
#include "NW_Device.h"

/// Missing or failed value on file, for every NW library (NOAA and earth-science convention).
#ifndef NW_ERROR
#define NW_ERROR -9999
#endif

#endif

/**
 * @file NW_Error.h
 * @brief The one missing-value sentinel every NW library writes to file.
 */
#ifndef NW_Error_h
#define NW_Error_h

/// Missing or failed value on file, for every NW library (NOAA and earth-science convention).
#ifndef NW_ERROR
#define NW_ERROR -9999
#endif

/// Scale a statistic from register units to physical units, passing NW_ERROR through untouched.
inline float nwScaled(float v, float divisor) { return (v == NW_ERROR) ? NW_ERROR : v / divisor; }

#endif

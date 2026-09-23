/**
 * @file NW_Readings.h
 * @brief A measurement's readings in a fixed-capacity array, with statistics.
 */
#ifndef NW_Readings_h
#define NW_Readings_h

#include <Arduino.h>
#include <math.h>
#include "NW_Error.h"

/**
 * @brief Fixed-capacity store for one measurement's readings, with statistics.
 * @details One storage path (LIBRARY-DESIGN, decided 2026-09-22): every
 * acquisition appends here; the scalar getter of a library returns last();
 * the statistics are computed from the array on demand, so a batch logged
 * through logReading() has its statistics without a second acquisition.
 * Appending past CAPACITY overwrites the oldest reading (the newest CAPACITY
 * readings are kept). No heap. mean/std/sterr are two-pass in float, exact
 * enough for CAPACITY up to a few hundred; median copies the array once onto
 * the stack (CAPACITY * sizeof(T) bytes), so keep CAPACITY modest on small AVRs.
 * @tparam T        element type (int16_t, uint16_t, uint32_t, float)
 * @tparam CAPACITY number of readings kept
 */
template <typename T, uint16_t CAPACITY>
class NW_Readings {
  public:
    void reset()                      { _count = 0; _next = 0; }
    void append(T v) {
      _v[_next] = v;
      _next = (uint16_t)((_next + 1) % CAPACITY);
      if (_count < CAPACITY) _count++;
    }
    uint16_t count() const            { return _count; }
    static constexpr uint16_t capacity() { return CAPACITY; }
    /** @brief The newest reading (meaningful only if count() > 0). */
    T last() const                    { return _v[(uint16_t)((_next + CAPACITY - 1) % CAPACITY)]; }
    /** @brief The readings, in storage order (not chronological after a wrap; statistics do not care). */
    const T* data() const             { return _v; }

    float mean() const  { if (_count == 0) return NW_READINGS_EMPTY; float m, sd, se; _stats(m, sd, se); return m; }
    /** @brief Sample standard deviation (n-1); 0 for a single reading. */
    float std() const   { if (_count == 0) return NW_READINGS_EMPTY; float m, sd, se; _stats(m, sd, se); return sd; }
    /** @brief Standard error of the mean, std / sqrt(n); 0 for a single reading. */
    float sterr() const { if (_count == 0) return NW_READINGS_EMPTY; float m, sd, se; _stats(m, sd, se); return se; }
    /** @brief Median: copies the readings once and insertion-sorts (n <= CAPACITY). */
    float median() const {
      if (_count == 0) return NW_READINGS_EMPTY;
      T tmp[CAPACITY];
      for (uint16_t i = 0; i < _count; i++) tmp[i] = _v[i];
      for (uint16_t i = 1; i < _count; i++) {
        T x = tmp[i]; int16_t j = i - 1;
        while (j >= 0 && tmp[j] > x) { tmp[j + 1] = tmp[j]; j--; }
        tmp[j + 1] = x;
      }
      return (_count & 1) ? (float)tmp[_count / 2]
                : ((float)tmp[_count / 2 - 1] + (float)tmp[_count / 2]) / 2;
    }

    static constexpr float NW_READINGS_EMPTY = NW_ERROR;   ///< returned by the statistics when count() == 0: the file sentinel

  private:
    T _v[CAPACITY];
    uint16_t _count = 0;
    uint16_t _next  = 0;
    void _stats(float& mean, float& sd, float& se) const {
      float sum = 0;
      for (uint16_t i = 0; i < _count; i++) sum += _v[i];
      mean = sum / _count;
      float m2 = 0;
      for (uint16_t i = 0; i < _count; i++) { float d = _v[i] - mean; m2 += d * d; }
      sd = (_count > 1) ? sqrt(m2 / (_count - 1)) : 0;
      se = (_count > 1) ? sd / sqrt((float)_count) : 0;
    }
};


/**
 * @brief How many readings a chip group takes per updateMeasurements() and
 * whether its statistics columns print: one per chip group in a library.
 * @details set() clamps to the group's array capacity and returns what was
 * set; columns() is the one rule for printing std and sterr (enabled, and
 * more than one reading, since one reading has no statistics).
 */
struct NW_ReadingsConfig {
  uint16_t n = 1;
  bool stats = false;
  uint16_t set(uint16_t v, uint16_t capacity) { n = (v > capacity) ? capacity : v; return n; }
  bool columns() const { return stats && n > 1; }
};

#endif

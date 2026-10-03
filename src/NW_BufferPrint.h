/**
 * @file NW_BufferPrint.h
 * @brief A Print that fills a fixed buffer and says when it ran out.
 */
#ifndef NW_BufferPrint_h
#define NW_BufferPrint_h

#include <Arduino.h>

/**
 * @brief Collect printed output into a caller's char buffer.
 * @details What a device uses where a String used to be held: a buffer whose
 * size is fixed and known, rather than a heap object that can silently empty.
 * The buffer is kept null-terminated, so it can be printed or compared at any
 * point.
 *
 * Running out is the hazard a String has too, and the reason this reports it:
 * write() returns 0 when the byte did not fit, Print sums what it wrote, and
 * truncated() latches whether anything was dropped. A caller that ignores both
 * gets a short line and no warning, which is the failure this exists to end.
 */
class NW_BufferPrint : public Print {
public:
  using Print::write;  // Print's non-virtual write() overloads, which this class would otherwise hide

  /**
     * @param buffer where to collect.
     * @param capacity its size in bytes, terminator included.
     * @param append keep what the buffer already holds and add to it, rather
     *        than clearing it first.
     */
  NW_BufferPrint(char* buffer, size_t capacity, bool append = false)
    : _buffer(buffer), _capacity(capacity) {
    if (append) _length = strlen(_buffer);
    else if (_capacity) _buffer[0] = '\0';
  }

  size_t write(uint8_t c) override {
    if (_length + 1 >= _capacity) {
      _truncated = true;
      return 0;
    }
    _buffer[_length++] = (char)c;
    _buffer[_length] = '\0';
    return 1;
  }

  /// @brief How many bytes the buffer holds, terminator excluded.
  size_t length() const {
    return _length;
  }

  /// @brief Whether anything was dropped for want of room.
  bool truncated() const {
    return _truncated;
  }

private:
  char* _buffer;
  size_t _capacity;
  size_t _length = 0;
  bool _truncated = false;
};

#endif

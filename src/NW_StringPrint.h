/**
 * @file NW_StringPrint.h
 * @brief A Print that appends to an Arduino String, and reports a failed append.
 */
#ifndef NW_StringPrint_h
#define NW_StringPrint_h

#include <Arduino.h>

/**
 * @brief Collect printed output into a String, with the failure visible.
 * @details The String-returning functions in a sensor library (getHeader() and
 * getString()) are implemented over the streaming pair by printing into one of
 * these, which leaves one definition of the column set. Two things make this
 * more than a convenience wrapper over operator +=:
 *
 * (1) It reports failure. The core's operator += discards concat()'s return
 * value, so a header cell that cannot be allocated is simply absent and no
 * caller can tell. write() returns the bytes it appended, Print sums those, and
 * failed() latches whether anything was lost.
 *
 * (2) It reserves in blocks. String::concat() reserves exactly what it needs,
 * which would reallocate once per character on the per-character path Print
 * takes for numbers. The block is the widest single value the core formats (its
 * float path writes through a 33-byte buffer), so no one value straddles a
 * growth boundary.
 *
 * See NW-Device-Specification/LIBRARY-DESIGN.md sections 14 and 15.
 */
class NW_StringPrint : public Print {
  public:
    using Print::write;   // Print's non-virtual write() overloads, which this class would otherwise hide

    /// @param out The String to append to. It is not cleared.
    explicit NW_StringPrint(String& out) : _out(out) {}

    size_t write(uint8_t c) override {
      if (!grow(1)) return 0;
      if (!_out.concat((char)c)) {
        _failed = true;
        return 0;
      }
      return 1;
    }

    size_t write(const uint8_t* buffer, size_t size) override {
      if (size == 0) return 0;
      if (!grow(size)) return 0;
      for (size_t i = 0; i < size; i++) {
        if (!_out.concat((char)buffer[i])) {
          _failed = true;
          return i;
        }
      }
      return size;
    }

    /// @brief Whether any append has failed. A short row is otherwise silent.
    bool failed() const {
      return _failed;
    }

  private:
    static const size_t Block = 32;   ///< Growth block: the widest value Print formats in one call

    bool grow(size_t n) {
      if (_out.reserve((unsigned int)(_out.length() + n + Block))) return true;
      _failed = true;
      return false;
    }

    String& _out;
    bool _failed = false;
};

#endif

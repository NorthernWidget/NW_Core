// Minimal host-side stand-in for the Arduino core, enough to compile and run an
// NW library on a desktop for output-regression testing. Not a general emulator.
// Shared by every NW library's harness: include it from NW_Core/extras/test.
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>

// Pin-level constants, the AVR core's values.
#define HIGH 0x1
#define LOW  0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

// PROGMEM strings: on the host they are ordinary strings.
#define PROGMEM
#define F(x) x
#define PSTR(x) x

// Arduino's binary.h: the Bxxxxxxxx constants, which the cores define and which
// the older NW sources use for register masks. All 256 eight-bit forms, as
// there.
// The cores' bit helpers, which the older NW sources use.
#ifndef bitRead
#define bit(b) (1UL << (b))
#define bitRead(value, b) (((value) >> (b)) & 0x01)
#define bitSet(value, b) ((value) |= (1UL << (b)))
#define bitClear(value, b) ((value) &= ~(1UL << (b)))
#define bitWrite(value, b, bitvalue) ((bitvalue) ? bitSet(value, b) : bitClear(value, b))
#endif

#ifndef B00000000
#define B00000000 0x00
#define B00000001 0x01
#define B00000010 0x02
#define B00000011 0x03
#define B00000100 0x04
#define B00000101 0x05
#define B00000110 0x06
#define B00000111 0x07
#define B00001000 0x08
#define B00001001 0x09
#define B00001010 0x0A
#define B00001011 0x0B
#define B00001100 0x0C
#define B00001101 0x0D
#define B00001110 0x0E
#define B00001111 0x0F
#define B00010000 0x10
#define B00010001 0x11
#define B00010010 0x12
#define B00010011 0x13
#define B00010100 0x14
#define B00010101 0x15
#define B00010110 0x16
#define B00010111 0x17
#define B00011000 0x18
#define B00011001 0x19
#define B00011010 0x1A
#define B00011011 0x1B
#define B00011100 0x1C
#define B00011101 0x1D
#define B00011110 0x1E
#define B00011111 0x1F
#define B00100000 0x20
#define B00100001 0x21
#define B00100010 0x22
#define B00100011 0x23
#define B00100100 0x24
#define B00100101 0x25
#define B00100110 0x26
#define B00100111 0x27
#define B00101000 0x28
#define B00101001 0x29
#define B00101010 0x2A
#define B00101011 0x2B
#define B00101100 0x2C
#define B00101101 0x2D
#define B00101110 0x2E
#define B00101111 0x2F
#define B00110000 0x30
#define B00110001 0x31
#define B00110010 0x32
#define B00110011 0x33
#define B00110100 0x34
#define B00110101 0x35
#define B00110110 0x36
#define B00110111 0x37
#define B00111000 0x38
#define B00111001 0x39
#define B00111010 0x3A
#define B00111011 0x3B
#define B00111100 0x3C
#define B00111101 0x3D
#define B00111110 0x3E
#define B00111111 0x3F
#define B01000000 0x40
#define B01000001 0x41
#define B01000010 0x42
#define B01000011 0x43
#define B01000100 0x44
#define B01000101 0x45
#define B01000110 0x46
#define B01000111 0x47
#define B01001000 0x48
#define B01001001 0x49
#define B01001010 0x4A
#define B01001011 0x4B
#define B01001100 0x4C
#define B01001101 0x4D
#define B01001110 0x4E
#define B01001111 0x4F
#define B01010000 0x50
#define B01010001 0x51
#define B01010010 0x52
#define B01010011 0x53
#define B01010100 0x54
#define B01010101 0x55
#define B01010110 0x56
#define B01010111 0x57
#define B01011000 0x58
#define B01011001 0x59
#define B01011010 0x5A
#define B01011011 0x5B
#define B01011100 0x5C
#define B01011101 0x5D
#define B01011110 0x5E
#define B01011111 0x5F
#define B01100000 0x60
#define B01100001 0x61
#define B01100010 0x62
#define B01100011 0x63
#define B01100100 0x64
#define B01100101 0x65
#define B01100110 0x66
#define B01100111 0x67
#define B01101000 0x68
#define B01101001 0x69
#define B01101010 0x6A
#define B01101011 0x6B
#define B01101100 0x6C
#define B01101101 0x6D
#define B01101110 0x6E
#define B01101111 0x6F
#define B01110000 0x70
#define B01110001 0x71
#define B01110010 0x72
#define B01110011 0x73
#define B01110100 0x74
#define B01110101 0x75
#define B01110110 0x76
#define B01110111 0x77
#define B01111000 0x78
#define B01111001 0x79
#define B01111010 0x7A
#define B01111011 0x7B
#define B01111100 0x7C
#define B01111101 0x7D
#define B01111110 0x7E
#define B01111111 0x7F
#define B10000000 0x80
#define B10000001 0x81
#define B10000010 0x82
#define B10000011 0x83
#define B10000100 0x84
#define B10000101 0x85
#define B10000110 0x86
#define B10000111 0x87
#define B10001000 0x88
#define B10001001 0x89
#define B10001010 0x8A
#define B10001011 0x8B
#define B10001100 0x8C
#define B10001101 0x8D
#define B10001110 0x8E
#define B10001111 0x8F
#define B10010000 0x90
#define B10010001 0x91
#define B10010010 0x92
#define B10010011 0x93
#define B10010100 0x94
#define B10010101 0x95
#define B10010110 0x96
#define B10010111 0x97
#define B10011000 0x98
#define B10011001 0x99
#define B10011010 0x9A
#define B10011011 0x9B
#define B10011100 0x9C
#define B10011101 0x9D
#define B10011110 0x9E
#define B10011111 0x9F
#define B10100000 0xA0
#define B10100001 0xA1
#define B10100010 0xA2
#define B10100011 0xA3
#define B10100100 0xA4
#define B10100101 0xA5
#define B10100110 0xA6
#define B10100111 0xA7
#define B10101000 0xA8
#define B10101001 0xA9
#define B10101010 0xAA
#define B10101011 0xAB
#define B10101100 0xAC
#define B10101101 0xAD
#define B10101110 0xAE
#define B10101111 0xAF
#define B10110000 0xB0
#define B10110001 0xB1
#define B10110010 0xB2
#define B10110011 0xB3
#define B10110100 0xB4
#define B10110101 0xB5
#define B10110110 0xB6
#define B10110111 0xB7
#define B10111000 0xB8
#define B10111001 0xB9
#define B10111010 0xBA
#define B10111011 0xBB
#define B10111100 0xBC
#define B10111101 0xBD
#define B10111110 0xBE
#define B10111111 0xBF
#define B11000000 0xC0
#define B11000001 0xC1
#define B11000010 0xC2
#define B11000011 0xC3
#define B11000100 0xC4
#define B11000101 0xC5
#define B11000110 0xC6
#define B11000111 0xC7
#define B11001000 0xC8
#define B11001001 0xC9
#define B11001010 0xCA
#define B11001011 0xCB
#define B11001100 0xCC
#define B11001101 0xCD
#define B11001110 0xCE
#define B11001111 0xCF
#define B11010000 0xD0
#define B11010001 0xD1
#define B11010010 0xD2
#define B11010011 0xD3
#define B11010100 0xD4
#define B11010101 0xD5
#define B11010110 0xD6
#define B11010111 0xD7
#define B11011000 0xD8
#define B11011001 0xD9
#define B11011010 0xDA
#define B11011011 0xDB
#define B11011100 0xDC
#define B11011101 0xDD
#define B11011110 0xDE
#define B11011111 0xDF
#define B11100000 0xE0
#define B11100001 0xE1
#define B11100010 0xE2
#define B11100011 0xE3
#define B11100100 0xE4
#define B11100101 0xE5
#define B11100110 0xE6
#define B11100111 0xE7
#define B11101000 0xE8
#define B11101001 0xE9
#define B11101010 0xEA
#define B11101011 0xEB
#define B11101100 0xEC
#define B11101101 0xED
#define B11101110 0xEE
#define B11101111 0xEF
#define B11110000 0xF0
#define B11110001 0xF1
#define B11110010 0xF2
#define B11110011 0xF3
#define B11110100 0xF4
#define B11110101 0xF5
#define B11110110 0xF6
#define B11110111 0xF7
#define B11111000 0xF8
#define B11111001 0xF9
#define B11111010 0xFA
#define B11111011 0xFB
#define B11111100 0xFC
#define B11111101 0xFD
#define B11111110 0xFE
#define B11111111 0xFF
#endif

#define pgm_read_byte(p) (*(const uint8_t*)(p))

inline uint32_t& _millis_counter() { static uint32_t t = 0; return t; }
inline uint32_t millis() { return _millis_counter(); }
inline void delay(uint32_t ms) { _millis_counter() += ms; }

// avr-libc dtostrf(val, width, prec, buf) is sprintf("%*.*f").
inline char* dtostrf(double val, signed char width, unsigned char prec, char* buf) {
  sprintf(buf, "%*.*f", width, prec, val); return buf;
}

// Arduino String: only what Apis uses. Float construction mirrors WString.cpp:
// dtostrf(value, decimalPlaces + 2, decimalPlaces, buf).
class String {
  std::string s;
  public:
  String() {}
  String(const char* c) : s(c) {}
  String(const std::string& c) : s(c) {}
  String(int v) : s(std::to_string(v)) {}
  String(long v) : s(std::to_string(v)) {}
  String(unsigned int v) : s(std::to_string(v)) {}
  String(unsigned long v) : s(std::to_string(v)) {}
  String(float v, unsigned char d = 2)  { char b[33]; dtostrf(v, d + 2, d, b); s = b; }
  String(double v, unsigned char d = 2) { char b[33]; dtostrf(v, d + 2, d, b); s = b; }
  String operator+(const String& o) const { return String(s + o.s); }
  String operator+(const char* o) const { return String(s + o); }
  String& operator+=(const String& o) { s += o.s; return *this; }
  String& operator+=(const char* o) { s += o; return *this; }
  const char* c_str() const { return s.c_str(); }
  bool operator==(const String& o) const { return s == o.s; }
  bool operator==(const char* o) const   { return s == o; }
  bool operator!=(const String& o) const { return s != o.s; }
  bool operator!=(const char* o) const   { return s != o; }
  // The real String's growth and append primitives, which NW_StringPrint uses.
  // A host has a real heap, so both always succeed here.
  unsigned int length() const { return (unsigned int)s.size(); }
  unsigned char reserve(unsigned int n) { s.reserve(n); return 1; }
  unsigned char concat(char c) { s += c; return 1; }
};
inline String operator+(const char* a, const String& b) { return String(std::string(a) + b.c_str()); }

// Print: enough for printReading(Print&) once it exists.
class Print {
  public:
  virtual size_t write(uint8_t c) = 0;
  virtual size_t write(const uint8_t* b, size_t n) { size_t k = 0; while (n--) k += write(*b++); return k; }
  size_t print(const char* c) { return write((const uint8_t*)c, strlen(c)); }
  size_t print(char c) { return write((uint8_t)c); }
  size_t print(int v) { char b[16]; sprintf(b, "%d", v); return print(b); }
  size_t print(long v) { char b[24]; sprintf(b, "%ld", v); return print(b); }
  size_t print(unsigned int v) { char b[16]; sprintf(b, "%u", v); return print(b); }
  size_t print(unsigned long v) { char b[24]; sprintf(b, "%lu", v); return print(b); }
  size_t print(const String& v) { return print(v.c_str()); }
  // Print::printFloat (AVR core): round then print integer part, '.', digits.
  size_t print(double v, int digits = 2) {
    size_t n = 0;
    if (std::isnan(v)) return print("nan");
    if (std::isinf(v)) return print("inf");
    if (v < 0.0) { n += print('-'); v = -v; }
    double rounding = 0.5; for (int i = 0; i < digits; i++) rounding /= 10.0;
    v += rounding;
    unsigned long ip = (unsigned long)v; double rem = v - (double)ip;
    n += print(ip);
    if (digits > 0) n += print('.');
    while (digits-- > 0) { rem *= 10.0; unsigned int d = (unsigned int)rem; n += print(d); rem -= d; }
    return n;
  }
  size_t println() { return print("\n"); }
  template <typename T> size_t println(T v) { size_t n = print(v); return n + print("\n"); }
  template <typename T, typename U> size_t println(T v, U base) { size_t n = print(v, base); return n + print("\n"); }
};

// Serial: a Print that writes to stdout, so a library's debug helpers link.
// Harness output that matters goes through printf; keep Serial out of baselines.
#define HEX 16
#define DEC 10
class SerialStub : public Print {
  public:
  void begin(unsigned long) {}
  size_t write(uint8_t c) override { return (size_t)fputc(c, stdout) != EOF; }
  size_t print(unsigned long v, int base) { char b[24]; snprintf(b, sizeof b, base == 16 ? "%lX" : "%lu", v); return print(b); }
  size_t print(int v, int base) { return print((unsigned long)v, base); }
  using Print::print;
};
static SerialStub Serial;

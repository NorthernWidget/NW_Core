// Host-side stand-in for the AVR EEPROM library: a byte array with read, write,
// update and length, enough for NW_Pages::loadStored() and a logger's Page 0.
#pragma once
#include <cstdint>
#include <cstring>
class EEPROMClass {
  public:
  uint8_t cells[4096];
  EEPROMClass() { memset(cells, 0xFF, sizeof cells); }
  uint8_t read(int a) const { return cells[a & 0xFFF]; }
  void write(int a, uint8_t v) { cells[a & 0xFFF] = v; }
  void update(int a, uint8_t v) { cells[a & 0xFFF] = v; }
  int length() const { return 4096; }
};
static EEPROMClass EEPROM;

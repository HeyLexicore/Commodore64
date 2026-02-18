#include "Memory.h"
#include <cstdint>

Rom::Rom(uint16_t size){ mem.resize(size, 0); size_ = size; };
Ram::Ram(uint16_t size){ mem.resize(size, 0); size_ = size; };

uint8_t Rom::read8(uint16_t addr){
  return mem[addr];
};

uint8_t Ram::read8(uint16_t addr){
  return mem[addr];
};

void Ram::write8(uint16_t addr, uint8_t val){
  mem[addr] = val;
};


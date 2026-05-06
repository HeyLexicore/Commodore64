
#pragma once
#include <cstdint>
class Vic{
  private:
    int raster_line = 0;
    int count = 1;
    uint8_t MEMIO[0x0400];

  public: 

    int max_rasterlines = 311;
    Vic(){};

    void advance_rasterline(int c);
    uint8_t& read8(uint16_t addr);
    void write8(uint16_t addr, uint8_t val);
};

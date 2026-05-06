#include "Vic.h"
#include <cstdint>
#include <stdexcept>

uint8_t& Vic::read8(uint16_t addr){
  
  MEMIO[0x0011] = (MEMIO[0x0011] & 0b11111110) | ((raster_line & 0b100000000) > 8);
  MEMIO[0x0012] = raster_line & 0xFF;

  return MEMIO[addr];
}

void Vic::write8(uint16_t addr, uint8_t val){

}

void Vic::advance_rasterline(int c){
  count += c;
  if (count >= 63){
    raster_line ++;
    if (raster_line > max_rasterlines){
      raster_line = 0;
    }
  }
}

#pragma once
#include "common.h"
#include <cstdint>
#include <string>
#include <vector>
#include "Vic.h"

class Bus{
  private:
    uint8_t RAM[0x10000]; 
    uint8_t KERNAL[0x2000];
    uint8_t BASIC[0x2000];
    uint8_t CHAR[0x1000];
    uint8_t MEMIO[0x1000];
    uint8_t tVal;
    settings set;

    Vic* vic;
  public:

    void print_status(); 
    void write8(uint16_t addr, uint8_t val);
    uint8_t& read8(uint16_t addr);
    uint8_t& read8_silent(uint16_t addr);

    void write16(uint16_t, uint16_t val);
    uint16_t read16(uint16_t);
    
    void writeKernal(std::string name);
    void writeBasic(std::string name);
    void defaults();
    void setupVic(Vic*);

    Bus(settings set):set(set){};
    Bus(){};
};



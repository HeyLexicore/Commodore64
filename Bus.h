#pragma once

#include "Device.h"
#include <cstdint>
#include <vector>


class Bus{
  private:
    std::vector<Device*> devices;
    std::vector<uint16_t> startAddrs;
    uint last_addr = 0;

  public:
    
    void Register(Device* dev);
    
    void write8(uint16_t addr, uint8_t val);
    uint8_t read8(uint16_t addr);
    Bus(){};
};



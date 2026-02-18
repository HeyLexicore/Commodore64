#pragma once

#include <iostream>
#include <cstdint>
#include <stdexcept>


class Device{

  public:
    virtual void write8(uint16_t addr, uint8_t val){ throw std::runtime_error("Not implemnted write8"); };
    virtual uint8_t read8(uint16_t addr){ throw std::runtime_error("Not implemented read8"); };
    
    virtual uint16_t size() { throw std::runtime_error("Uhh no");};
    
    Device(){};
    
    virtual ~Device() = default;
};

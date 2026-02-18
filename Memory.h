#include "Device.h"
#include <cstdint>
#include <vector>

class Rom : public Device{
  protected:
    std::vector<uint8_t> mem;


  public:

    uint16_t size_;

    uint8_t read8(uint16_t addr) ;
    uint16_t size(){ return size_; }

    Rom(uint16_t size);
    Rom(){};

};

class Ram : public Device{
  protected:
    std::vector<uint8_t> mem;

  public:

    uint16_t size_;
    uint16_t size(){ return size_; }

    uint8_t read8(uint16_t addr) ;
    void write8(uint16_t addr, uint8_t val);
    
    Ram(uint16_t size);
    Ram(){};


};

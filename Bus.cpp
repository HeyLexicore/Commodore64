#include "Bus.h"
#include <cstdint>
#include <cstdio>

void Bus::Register(Device* dev){
  last_addr += dev->size();
  //printf("%02x\n",last_addr);
  startAddrs.push_back(last_addr);
  devices.push_back(dev);

};
void Bus::write8(uint16_t addr, uint8_t val){
  int res = 0;
  for (int i = 0; i < (int)startAddrs.size(); i++){
    //printf("%i\n",startAddrs[i]); 
    if (startAddrs[i] > addr){
      res = i-1;
    }
  };
  printf("a%i %i %04x %04x\n",res,(int)startAddrs.size(), addr, startAddrs[res]);
  //devices[res]->write8(addr-startAddrs[res], val);


};
uint8_t Bus::read8(uint16_t addr){

  int res = 0;
  for (int i = 0; i < (int)startAddrs.size(); i++){
    //printf("%i\n",startAddrs[i]); 
    if (startAddrs[i] > addr){
      res = i-1;
    }
  };


  return devices[res]->read8(addr-startAddrs[res]);

};


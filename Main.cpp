#include "Bus.h"
#include "Memory.h"
#include "Device.h"
#include <cstdio>
#include <memory>



int main (int argc, char *argv[]) {
  
  Device* hiRamPtr;
  Device* loRamPtr;
  Device* basicPtr;
  Device* kernalPtr;
  
  Ram loRam(0xA000);
  Ram hiRam(0x1000);

  Rom basic(0x2000);
  Rom kernal(0x2000);

  hiRamPtr = &hiRam;
  loRamPtr = &loRam;
  basicPtr = &basic;
  kernalPtr = &kernal;
  
  Bus bus;

  bus.Register(loRamPtr);
  bus.Register(basicPtr);
  bus.Register(hiRamPtr);
  bus.Register(kernalPtr);
  
  bus.write8(0xC000, 0x10);
  //printf("%i",bus.read8(0xC000));

  return 0;
}

#include "Bus.h"
#include "Vic.h"
#include "common.h"
#include <cstdint>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>

void Bus::defaults(){ // https://sta.c64.org/cbm64mem.html if nothing else works, fill up all 57 defauls
  write8(0x0000, 0x2F); //Processor port data direction register
  write8(0x0001, 0x37); // Processor port
};
void Bus::setupVic(Vic* vic_){
  vic = vic_;
}

void Bus::print_status(){
  printf("Bus status: %02X\n",RAM[0x0001]);



  bool kernalEN = (RAM[0x0001] & 0b010) >> 1;
  bool basicEN  = (RAM[0x0001] & 0b001);
  bool inoutEN  = (RAM[0x0001] & 0b100) >> 2;
  bool charEN   = (RAM[0x0001] & 0b111) == 3;
  bool allRAM   = (RAM[0x0001] & 0b111) == 0;

  if (kernalEN) printf("\tKernal enabled\n");
  if (basicEN) printf("\tBasic enabled\n");
  if (inoutEN) printf("\tMemory mapped io enabled\n");
  if (charEN) printf("\tChar mem enabled\n");
  if (allRAM) printf("\tAll ram\n");
}

uint8_t& Bus::read8(uint16_t addr){
  //check for all the diffrent banking types
  bool kernalEN = (RAM[0x0001] & 0b010) >> 1;
  bool basicEN  = (RAM[0x0001] & 0b001);
  bool inoutEN  = (RAM[0x0001] & 0b100) >> 2;
  bool charEN   = (RAM[0x0001] & 0b111) == 3;
  bool allRAM   = (RAM[0x0001] & 0b111) == 0;
  
  if (set.printSuperverbose){
    printf("Reading from %04X\n",addr);
  }
  //run through all the diffrent banking types
  if (allRAM){
    return RAM[addr];
  }

  if (addr >= 0xA000 & addr < 0xC000 & basicEN){ //If it is in basic and the basic bit is set, return basic else ignore
    return BASIC[addr-0xA000];
  } 

  if (addr >= 0xE000 & addr < 0x10000 & kernalEN){ //if it is in kernal and the kernal bit is set, return kernal else ignore
    return KERNAL[addr-0xE000];
  }
  
  if (addr >= 0xD000 & addr < 0xE000){ // if 
    if (inoutEN){
      if (set.printVerbose) printf("Reading MEMIO at address %04X\n",addr);
      
      if (addr >= 0xD000 & addr <= 0xD3FF){ // check if address is in vic if it is in texture make it normal ram

         return vic->read8(addr-0xD000);
      }
      
    } else if (charEN){
      if (set.printVerbose) printf("Reading CHARMEM at address %04X\n",addr);
      return  CHAR[addr-0xD000];
    }
  }
  return RAM[addr];
};
uint8_t& Bus::read8_silent(uint16_t addr){
  //check for all the diffrent banking types
  bool kernalEN = (RAM[0x0001] & 0b010) >> 1;
  bool basicEN  = (RAM[0x0001] & 0b001);
  bool inoutEN  = (RAM[0x0001] & 0b100) >> 2;
  bool charEN   = (RAM[0x0001] & 0b111) == 3;
  bool allRAM   = (RAM[0x0001] & 0b111) == 0;
  

  //run through all the diffrent banking types
  if (allRAM){
    return RAM[addr];
  }

  if (addr >= 0xA000 & addr < 0xC000 & basicEN){ //If it is in basic and the basic bit is set, return basic else ignore
    return BASIC[addr-0xA000];
  } 

  if (addr >= 0xE000 & addr < 0x10000 & kernalEN){ //if it is in kernal and the kernal bit is set, return kernal else ignore
    return KERNAL[addr-0xE000];
  }
  
  if (addr >= 0xD000 & addr < 0xE000){ // if 
    if (inoutEN){
      return MEMIO[addr-0xD000];
    } else if (charEN){
      return  CHAR[addr-0xD000];
    }
  }
  return RAM[addr];
};

void Bus::write8(uint16_t addr, uint8_t val){
   
  bool kernalEN = (RAM[0x0001] & 0b010) >> 1;
  bool basicEN  = (RAM[0x0001] & 0b001);
  bool inoutEN  = (RAM[0x0001] & 0b100) >> 2;
  bool charEN   = (RAM[0x0001] & 0b111) == 3;
  bool allRAM   = (RAM[0x0001] & 0b111) == 0;

  if (allRAM){
    RAM[addr] = val;
  }

  if (addr >= 0xA000 & addr < 0xC000 & basicEN){ //commented out because its read only
    //BASIC[addr-0xA000] = val;
    printf("Writing to read-only, basic %04X \n",addr);
  } 

  if (addr >= 0xE000 & addr < 0x10000 & kernalEN){ // same here
    //KERNAL[addr-0xE000] = val;
    printf("Writing to read-only, kernal %04X\n",addr);
  }
  
  if (addr >= 0xD000 & addr < 0xE000){ // if 
    if (inoutEN){ //if io memory/vic enabled, do that if charmem then that and the ram was before that is handled in up
      if (addr >= 0xD000 & addr <= 0xD3FF){ // check if address is in vic if it is in texture make it normal ram
        if (addr <= 0xD02E){
          if (set.printVerbose) printf("Writing to Vic %04X %02X\n",addr,val);
          vic->write8(addr-0xD000,val);
        } else {
          MEMIO[addr-0xD000] = val;
        }
      }

    } else if (charEN){
      if (set.printVerbose) printf("Writing to CHARMEM %04X\n",addr); // implement vic herre or like some pointer to the vic or some funny buissness

      CHAR[addr-0xD000] = val;
    }
  }
  
  RAM[addr] = val;
  if ((addr == 0x0001) & set.printVerbose) print_status();
  if (set.printSuperverbose){
    printf("Written %02X to %04X\n",val,addr);
  } 
};

void Bus::write16(uint16_t addr, uint16_t val)
{
    write8(addr, val & 0xFF);
    write8(addr + 1, val >> 8);
};

uint16_t Bus::read16(uint16_t addr)
{
    return read8(addr) | (read8(addr + 1) << 8);
};

void Bus::writeBasic(std::string name){
    
  std::ifstream file(name, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(&BASIC), size); 
  file.close();

}


void Bus::writeKernal(std::string name){
    
  std::ifstream file(name, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(&KERNAL), size); 
  file.close();

}

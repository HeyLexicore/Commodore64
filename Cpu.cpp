#include "Cpu.h"
#include "Bus.h"
#include <cstdint>

#define IMPLIED 0
#define ACCUMULATOR 1
#define IMMEDIATE 2
#define ABSOLUTE 3
#define XINDEXEDABSOLUTE 4
#define YINDEXEDABSOLUTE 5
#define ABSOLUTEINDIRECT 6
#define ZEROPAGE 7
#define XINDEXEDZEROPAGE 8
#define YINDEXEDZEROPAGE 9
#define XINDEXEDZEROPAGEINDIRECT 10
#define ZEROPAGEINDIRECTYINDEXED 11
#define RELATIVE 12


Cpu::Cpu(Bus* bus): bus(bus) {};

void Cpu::setJK(){ // sets the arguments to the opcode byte 2 and byte 3
  J = bus->read8(PC+1);
  K = bus->read8(PC+2);
};
uint16_t Cpu::combineJK(){ //combines them for convienience sake, little endian
  return J | (K << 8);
};

uint8_t* Cpu::addrCallType(int type){ //returns a pointer to the actual thing to modify and its maybe a register or some other dumb stuff i hate it but it works
  setJK();
  switch (type) {
    case IMPLIED:
      break;
    case ACCUMULATOR:
      return &A;
    case IMMEDIATE:
      return &J;
    case ABSOLUTE:
      return &bus->read8(combineJK());
    case XINDEXEDABSOLUTE:
      return &bus->read8(combineJK()+X);
    case YINDEXEDABSOLUTE:
      return &bus->read8(combineJK()+Y);
    case ABSOLUTEINDIRECT:
      break; //fuckass exception only used once lol
    case ZEROPAGE:
      return &bus->read8(J);
    case XINDEXEDZEROPAGE:
      return &bus->read8(J+X);
    case YINDEXEDZEROPAGE:
      return &bus->read8(J+Y);
    case XINDEXEDZEROPAGEINDIRECT:
      return &bus->read8( bus->read8(J+X) | (bus->read8(J+X+1) << 8));
    case ZEROPAGEINDIRECTYINDEXED:
      return &bus->read8(uint16_t(bus->read8(J)) + ((uint16_t)bus->read8(J+1) << 8) + Y);
    case RELATIVE:
      break;
    default:
      break;

  };

  return nullptr;
};

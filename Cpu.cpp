#include "Cpu.h"
#include "Bus.h"
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include "Opcodes.h"

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


void Cpu::setupOpcodes(){
  for (int j = 0; j<56; j++){
    for (int k = 0; k < 13; k++){
      int val = rawTable[j][k];
      if (val != -1){
        opcodeCode[val] = j; opcodeMode[val] = k; opcodeMnem[val] = mnemonics[j];
      }
    }
  }
};

void Cpu::initExec(){
  opcodeExecution[0] = &Cpu::ADC;
  opcodeExecution[1] = &Cpu::
}

void Cpu::setNZ(uint8_t val){
  P.N = ((val & 128) > 0);
  P.Z = (val == 0);
}

void Cpu::jumpRelative(bool jump){
  if (jump) {
    PC = uint16_t(static_cast<int8_t>(J) + PC);
  } 
};

#define NOT_IMPLEMENTED throw std::runtime_error("Not implented");
#define val *addrCallType(mode)

void Cpu::LDA(int mode){A = val;}; 
void Cpu::LDX(int mode){X = val;};
void Cpu::LDY(int mode){Y = val;};
void Cpu::STA(int mode){val = A;};
void Cpu::STX(int mode){val = X;};
void Cpu::STY(int mode){val = Y;};

void Cpu::TAX(int mode){X = A;};
void Cpu::TAY(int mode){Y = A;};
void Cpu::TSX(int mode){X = SP;};
void Cpu::TXA(int mode){A = X;};
void Cpu::TXS(int mode){SP = X;};
void Cpu::TYA(int mode){A = Y;};

void Cpu::stackPush(uint8_t value){
  bus->write8(uint16_t(SP) + 0x0100, value);
  SP--;
};

uint8_t Cpu::stackPull(){
  SP++;
  return bus->read8(uint16_t(SP) + 0x0100);
};

void Cpu::PHA(int mode){stackPush(A);};
void Cpu::PHP(int mode){stackPush(P.pack());};
void Cpu::PLA(int mode){A = stackPull();};
void Cpu::PLP(int mode){P.unpack(stackPull());};

void Cpu::ASL(int mode){NOT_IMPLEMENTED};
void Cpu::LSR(int mode){

  P.C = val & 0x01;
  val >>= 1;
 
  setNZ(val);
  P.N = 0;
};
void Cpu::ROL(int mode){NOT_IMPLEMENTED};
void Cpu::ROR(int mode){NOT_IMPLEMENTED};

void Cpu::AND(int mode){NOT_IMPLEMENTED};
void Cpu::BIT(int mode){NOT_IMPLEMENTED};
void Cpu::EOR(int mode){NOT_IMPLEMENTED};
void Cpu::ORA(int mode){NOT_IMPLEMENTED};
void Cpu::ADC(int mode){NOT_IMPLEMENTED};
void Cpu::CMP(int mode){NOT_IMPLEMENTED};
void Cpu::CPX(int mode){NOT_IMPLEMENTED};
void Cpu::CPY(int mode){NOT_IMPLEMENTED};
void Cpu::SBC(int mode){NOT_IMPLEMENTED};

void Cpu::DEC(int mode){NOT_IMPLEMENTED};
void Cpu::DEX(int mode){NOT_IMPLEMENTED};
void Cpu::DEY(int mode){NOT_IMPLEMENTED};
void Cpu::INC(int mode){NOT_IMPLEMENTED};
void Cpu::INX(int mode){NOT_IMPLEMENTED};
void Cpu::INY(int mode){NOT_IMPLEMENTED};

void Cpu::BRK(int mode){NOT_IMPLEMENTED};
void Cpu::JMP(int mode){NOT_IMPLEMENTED};
void Cpu::JSR(int mode){NOT_IMPLEMENTED};
void Cpu::RTI(int mode){NOT_IMPLEMENTED};
void Cpu::RTS(int mode){NOT_IMPLEMENTED};

void Cpu::BCC(int mode){NOT_IMPLEMENTED};
void Cpu::BCS(int mode){NOT_IMPLEMENTED};
void Cpu::BEQ(int mode){NOT_IMPLEMENTED};
void Cpu::BMI(int mode){NOT_IMPLEMENTED};
void Cpu::BNE(int mode){NOT_IMPLEMENTED};
void Cpu::BPL(int mode){NOT_IMPLEMENTED};
void Cpu::BVC(int mode){NOT_IMPLEMENTED};
void Cpu::BVS(int mode){NOT_IMPLEMENTED};

void Cpu::CLC(int mode){NOT_IMPLEMENTED};
void Cpu::CLD(int mode){NOT_IMPLEMENTED};
void Cpu::CLI(int mode){NOT_IMPLEMENTED};
void Cpu::CLV(int mode){NOT_IMPLEMENTED};
void Cpu::SEC(int mode){NOT_IMPLEMENTED};
void Cpu::SED(int mode){NOT_IMPLEMENTED};
void Cpu::SEI(int mode){NOT_IMPLEMENTED};

void Cpu::NOP(int mode){NOT_IMPLEMENTED};

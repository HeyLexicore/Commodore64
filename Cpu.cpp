#include "Cpu.h"
#include "Bus.h"
#include "common.h"
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

void Cpu::setJK(int m){ // sets the arguments to the opcode byte 2 and byte 3
  switch (m){
    case 1:
      break;
    case 3:
      K = bus->read8(PC+2);
    case 2:
      J = bus->read8(PC+1);
      break;
  }
};
uint16_t Cpu::combineJK(){ //combines them for convienience sake, little endian
  return J | (K << 8);
};

uint8_t Cpu::valueJK(int type){ //returns a pointer to the actual thing to modify and its maybe a register or some other dumb stuff i hate it but it works
  
  switch (type) {
    case IMPLIED:
      break;
    case ACCUMULATOR:
      return A;
      break;
    case IMMEDIATE:
      return J;
      break;
    case ABSOLUTE:
      return bus->read8(combineJK());
      break;
    case XINDEXEDABSOLUTE:
      return bus->read8(combineJK()+X);
      break;
    case YINDEXEDABSOLUTE:
      return bus->read8(combineJK()+Y);
      break;
    case ABSOLUTEINDIRECT:
      break; //fuckass exception only used once lol
    case ZEROPAGE:
      return bus->read8(J);
      break;
    case XINDEXEDZEROPAGE:
      return bus->read8(J+X);
      break;
    case YINDEXEDZEROPAGE:
      return bus->read8(J+Y);
      break;
    case XINDEXEDZEROPAGEINDIRECT:
      return bus->read8( bus->read8(J+X) | (bus->read8(J+X+1) << 8));
      break;
    case ZEROPAGEINDIRECTYINDEXED:
      return bus->read8(uint16_t(bus->read8(J)) + ((uint16_t)bus->read8(J+1) << 8) + Y);
      break;
    case RELATIVE:
      break;

  };
  return 0;
};
uint16_t Cpu::addressJK(int type){ //returns a pointer to the actual thing to modify and its maybe a register or some other dumb stuff i hate it but it works
  
  switch (type) {
    case IMPLIED:
      break;
    case ACCUMULATOR:
      throw std::runtime_error("address of ACCUMULATOR");
      break;
    case IMMEDIATE:
      throw std::runtime_error("address of IMMEDIATE");
      break;
    case ABSOLUTE:
      return combineJK();
      break;
    case XINDEXEDABSOLUTE:
      return combineJK()+X;
      break;
    case YINDEXEDABSOLUTE:
      return combineJK()+Y;
      break;
    case ABSOLUTEINDIRECT:
      break; //fuckass exception only used once lol
    case ZEROPAGE:
      return J;
      break;
    case XINDEXEDZEROPAGE:
      return J+X;
      break;
    case YINDEXEDZEROPAGE:
      return J+Y;
      break;
    case XINDEXEDZEROPAGEINDIRECT:
      return  bus->read8(J+X) | (bus->read8(J+X+1) << 8);
      break;
    case ZEROPAGEINDIRECTYINDEXED:
      return uint16_t(bus->read8(J)) + ((uint16_t)bus->read8(J+1) << 8) + Y;
      break;
    case RELATIVE:
      break;

  };
  return 0;
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

void Cpu::setupExecs(){
  opcodeExecution[0]=&Cpu::ADC;
  opcodeExecution[1]=&Cpu::AND;
  opcodeExecution[2]=&Cpu::ASL;
  opcodeExecution[3]=&Cpu::BCC;
  opcodeExecution[4]=&Cpu::BCS;
  opcodeExecution[5]=&Cpu::BEQ;
  opcodeExecution[6]=&Cpu::BIT;
  opcodeExecution[7]=&Cpu::BMI;
  opcodeExecution[8]=&Cpu::BNE;
  opcodeExecution[9]=&Cpu::BPL;
  opcodeExecution[10]=&Cpu::BRK;
  opcodeExecution[11]=&Cpu::BVC;
  opcodeExecution[12]=&Cpu::BVS;
  opcodeExecution[13]=&Cpu::CLC;
  opcodeExecution[14]=&Cpu::CLD;
  opcodeExecution[15]=&Cpu::CLI;
  opcodeExecution[16]=&Cpu::CLV;
  opcodeExecution[17]=&Cpu::CMP;
  opcodeExecution[18]=&Cpu::CPX;
  opcodeExecution[19]=&Cpu::CPY;
  opcodeExecution[20]=&Cpu::DEC;
  opcodeExecution[21]=&Cpu::DEX;
  opcodeExecution[22]=&Cpu::DEY;
  opcodeExecution[23]=&Cpu::EOR;
  opcodeExecution[24]=&Cpu::INC;
  opcodeExecution[25]=&Cpu::INX;
  opcodeExecution[26]=&Cpu::INY;
  opcodeExecution[27]=&Cpu::JMP;
  opcodeExecution[28]=&Cpu::JSR;
  opcodeExecution[29]=&Cpu::LDA;
  opcodeExecution[30]=&Cpu::LDX;
  opcodeExecution[31]=&Cpu::LDY;
  opcodeExecution[32]=&Cpu::LSR;
  opcodeExecution[33]=&Cpu::NOP;
  opcodeExecution[34]=&Cpu::ORA;
  opcodeExecution[35]=&Cpu::PHA;
  opcodeExecution[36]=&Cpu::PHP;
  opcodeExecution[37]=&Cpu::PLA;
  opcodeExecution[38]=&Cpu::PLP;
  opcodeExecution[39]=&Cpu::ROL;
  opcodeExecution[40]=&Cpu::ROR;
  opcodeExecution[41]=&Cpu::RTI;
  opcodeExecution[42]=&Cpu::RTS;
  opcodeExecution[43]=&Cpu::SBC;
  opcodeExecution[44]=&Cpu::SEC;
  opcodeExecution[45]=&Cpu::SED;
  opcodeExecution[46]=&Cpu::SEI;
  opcodeExecution[47]=&Cpu::STA;
  opcodeExecution[48]=&Cpu::STX;
  opcodeExecution[49]=&Cpu::STY;
  opcodeExecution[50]=&Cpu::TAX;
  opcodeExecution[51]=&Cpu::TAY;
  opcodeExecution[52]=&Cpu::TSX;
  opcodeExecution[53]=&Cpu::TXA;
  opcodeExecution[54]=&Cpu::TXS;
  opcodeExecution[55]=&Cpu::TYA;
}


uint8_t Cpu::fetch(){
  return bus->read8(PC);
}
void Cpu::advance_PC(){
  PC += opcodeLen[fetch()];
};
void Cpu::execute(uint8_t opcode) {
  int code = opcodeCode[opcode];
  int mode = opcodeMode[opcode];
  (this->*opcodeExecution[code])(mode);
}
int Cpu::cycle(settings set){
  uint8_t opcode = fetch(); 
  setJK(opcodeLen[opcode]);
  if (set.printOpcodes){
    printf("%s%s%s%s%s%s%s%s",
          P.N?"N":"-",
          P.V?"V":"-",
          P.EX?"-":"-",
          P.B?"B":"-",
          P.D?"D":"-",
          P.I?"I":"-",
          P.Z?"Z":"-",
          P.C?"C":"-");
    
    // what in the magic unicde is this
    printf(" \x1B[0;34m%04X \x1B[35m%04X \x1B[36mA %02X \x1B[32mX %02X \x1B[31mY %02X \x1B[34m%02X ",SP,PC, A, X, Y, opcode);
    
    printf("%s ",opcodeMnem[opcode].c_str());
    if (opcodeLen[opcode] >= 2)printf("%02X",J);
    if (opcodeLen[opcode] == 3)printf(" %02X",K);
    printf("\x1B[0m\n"); 

  }
  advance_PC();
  execute(opcode);
  return opcodeCycles[opcode]; 
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

void Cpu::setupReturnVector(){
  PC = bus->read16(0xFFFC);
}

#define NOT_IMPLEMENTED throw std::runtime_error("Not implented");
#define valJK valueJK(mode)
#define addJK addressJK(mode) 
void Cpu::LDA(int mode){A = valJK; setNZ(A);}; 
void Cpu::LDX(int mode){X = valJK; setNZ(X);};
void Cpu::LDY(int mode){Y = valJK; setNZ(Y);};
void Cpu::STA(int mode){bus->write8(addJK, A);};
void Cpu::STX(int mode){bus->write8(addJK, X);};
void Cpu::STY(int mode){bus->write8(addJK, Y);};

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

void Cpu::ASL(int mode){

  P.C = (valJK & 0x80) != 0; 
  uint8_t T = valJK << 1;    
  if (mode == 1){
    A = T;
  }else{
    bus->write8(addJK, T);
  }
  setNZ(T);                  
};
void Cpu::LSR(int mode){
  

  P.C = valJK & 0x01;
  uint8_t T = valJK >> 1;
  if (mode == 1){
    A = T;
  }else{
    bus->write8(addJK, T);
  }
  setNZ(T);
  P.N = 0;
};
void Cpu::ROL(int mode){
  bool oldC = P.C;
  P.C = 0b10000000 & valJK;
  uint8_t T = (valJK << 1) | P.C;
  if (mode == 1){
    A = T;
  }else{
    bus->write8(addJK, T);
  }
  setNZ(T);
};
void Cpu::ROR(int mode){
  uint8_t oldCarry = P.C;
  P.C = (valJK & 0x01) != 0;
  uint8_t T = (valJK >> 1) | (oldCarry << 7);
  if(mode == 1){
    A = T;
  }else{
    bus->write8(addJK, T);
  }

  setNZ(T);
};
void Cpu::AND(int mode){A = A & valJK; setNZ(A);};
void Cpu::BIT(int mode){
  uint8_t val = valJK;
  uint8_t result = A & val;
  P.Z = (result == 0);
  P.N = (val & 0x80);
  P.V = (val & 0x40);
};
void Cpu::EOR(int mode){A = A xor valJK; setNZ(A);};
void Cpu::ORA(int mode){A = A | valJK; setNZ(A);};
void Cpu::ADC(int mode){
  if (P.D){
    uint16_t sum = uint16_t(A)+uint16_t(valJK)+P.C;
    if ((sum & 0x0F) > 9) sum += 0x06;    // fix low nibble
    if ((sum & 0xF0) > 0x90) sum += 0x60;  // fix high nibble
    P.C = sum > 0xFF;
    A = uint8_t(sum);
    setNZ(A);


  }else{
    uint16_t sum = uint16_t(A)+uint16_t(valJK)+P.C;
    P.C = (sum > 0xFF);
    A = sum & 0xFF;
    setNZ(sum);
    P.V = ~(A ^ valJK) & (A ^ uint8_t(sum)) & 0x80;
  }
};
void Cpu::CMP(int mode){
  uint16_t T = uint16_t(A) - uint16_t(valJK);
  P.C = (A >= valJK);
  setNZ(T);
};
void Cpu::CPX(int mode){uint8_t T = X-valJK; setNZ(T); P.C = (T >= valJK);};
void Cpu::CPY(int mode){uint8_t T = Y-valJK; setNZ(T); P.C = (T >= valJK);};
void Cpu::SBC(int mode){
  uint8_t val = valJK;
  if (P.D){
    uint16_t diff = uint16_t(A) - uint16_t(val) - (P.C ? 0 : 1);

    uint16_t al = (A & 0x0F) - (val & 0x0F) - (P.C ? 0 : 1);
    uint16_t ah = (A >> 4) - (val >> 4);

    if (al & 0x10) {
      al -= 6;
      ah--;
    }

    if (ah & 0x10) {
      ah -= 6;
      P.C = 0; 
    } else {
      P.C = 1;
    }

    A = uint8_t(((ah << 4) | (al & 0x0F)) & 0xFF);
    setNZ(A);

    P.V = ((A ^ val) & (A ^ uint8_t(diff)) & 0x80);
  }
  else{
    uint16_t diff = uint16_t(A) - uint16_t(val) - (P.C ? 0 : 1);

    P.C = !(diff & 0x100);  
    A = uint8_t(diff);
    setNZ(A);

    P.V = (A ^ val) & (A ^ uint8_t(diff)) & 0x80;
  }
}

void Cpu::DEC(int mode){ bus->write8(addJK,valJK - 1); setNZ(valJK);};
void Cpu::DEX(int mode){ X--; setNZ(X);};
void Cpu::DEY(int mode){ Y--; setNZ(Y);};
void Cpu::INC(int mode){ bus->write8(addJK,valJK + 1); setNZ(valJK);};
void Cpu::INX(int mode){ X++; setNZ(X);};
void Cpu::INY(int mode){ Y++; setNZ(Y);};

void Cpu::BRK(int mode){NOT_IMPLEMENTED};
void Cpu::JMP(int mode){if (mode == 3)  {PC = combineJK();} else {PC = bus->read16(combineJK());}};
void Cpu::JSR(int mode){
  stackPush(uint8_t((PC & 0xFF00) >> 8));
  stackPush(uint8_t(PC & 0xFF));
  PC = combineJK();
};
void Cpu::RTI(int mode){NOT_IMPLEMENTED};
void Cpu::RTS(int mode){PC = uint16_t(stackPull()) | (uint16_t(stackPull()) << 8);};

void Cpu::BCC(int mode){jumpRelative(!P.C);};
void Cpu::BCS(int mode){jumpRelative(P.C);};
void Cpu::BEQ(int mode){jumpRelative(P.Z);};
void Cpu::BMI(int mode){jumpRelative(P.N);};
void Cpu::BNE(int mode){jumpRelative(!P.Z);};
void Cpu::BPL(int mode){jumpRelative(!P.N);};
void Cpu::BVC(int mode){jumpRelative(!P.V);};
void Cpu::BVS(int mode){jumpRelative(P.V);};

void Cpu::CLC(int mode){P.C = 0;};
void Cpu::CLD(int mode){P.D = 0;};
void Cpu::CLI(int mode){P.I = 0;};
void Cpu::CLV(int mode){P.V = 0;};
void Cpu::SEC(int mode){P.C = 1;};
void Cpu::SED(int mode){P.D = 1;};
void Cpu::SEI(int mode){P.I = 1;};

void Cpu::NOP(int mode){};

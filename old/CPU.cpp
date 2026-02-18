#include "CPU.h"
#include <cstdint>
#include <cstdlib>
#include <string>

#define NI abort();

uint8_t CPU::read8(const uint16_t addr){
  return memory[addr]; 
}

uint16_t CPU::read16LE(const uint16_t addr){
  return uint16_t(read8(addr)) | (uint16_t(read8(addr+1)) << 8);
}

void CPU::write8(const uint16_t addr, uint8_t val) {
  memory[addr] = val;
}

void CPU::stackPush(uint8_t val){
  write8(uint16_t(SP) + 0x0100, val);
  SP--;
}
uint8_t CPU::stackPull(){
  SP++;
  return read8(uint16_t(SP) + 0x0100);
}

void CPU::load_file(const char* filename, uint16_t offset){

  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  if (offset + size > sizeof(memory))
    throw std::runtime_error("File does not fit in memory");
  file.read(reinterpret_cast<char*>(&memory[offset]), size); 
  file.close();
}

uint8_t CPU::fetch(){
  return read8(PC);
}
void CPU::advance(){
  PC += opcodeLen[fetch()];
}

void CPU::print_opc(uint16_t addr){
  printf("%s",opcodeRepr[read8(addr)].c_str());
}

void CPU::setNZ(uint8_t val){
  P.N = ((val & 128) > 0);
  P.Z = (val == 0);
}

void CPU::execute(uint8_t opcode) { (this->*opcodeTable[opcode])(); }
// implied
void CPU::BRK(){ abort(); }; // $00
void CPU::CLC(){ P.C = 0; }; // $18
void CPU::CLD(){ P.D = 0; }; // $D8
void CPU::CLI(){ P.I = 0; }; // $58
void CPU::CLV(){ P.V = 0; }; // $B8
void CPU::DEX(){ X--; setNZ(X); }; // $CA
void CPU::DEY(){ Y--; setNZ(Y); }; // $88
void CPU::INX(){ X++; setNZ(X); }; // $E8
void CPU::INY(){ Y++; setNZ(Y); }; // $C8
void CPU::NOP(){}; // $EA
void CPU::PHA(){stackPush(A);}; // $48
void CPU::PHP(){stackPush(P.pack());}; // $08
void CPU::PLA(){A = stackPull();}; // $68
void CPU::PLP(){P.unpack(stackPull());}; // $28/
void CPU::RTI(){NI}; // $40
void CPU::RTS(){PC = uint16_t(stackPull()) | (uint16_t(stackPull()) << 8);}; // $60
void CPU::SEC(){ P.C = 1; }; // $38
void CPU::SED(){ P.D = 1; }; // $F8
void CPU::SEI(){ P.I = 1; }; // $78
void CPU::TAX(){ X = A; }; // $AA
void CPU::TAY(){ Y = A; }; // $A8
void CPU::TSX(){ X = SP;}; // $BA
void CPU::TXA(){ A = X; }; // $8A
void CPU::TXS(){ SP = X; }; // $9A
void CPU::TYA(){ A = Y; }; // $98
                           //
void CPU::ADC(uint8_t val){
  if (P.D){
    uint16_t sum = uint16_t(A)+uint16_t(val)+P.C;
    if ((sum & 0x0F) > 9) sum += 0x06;    // fix low nibble
    if ((sum & 0xF0) > 0x90) sum += 0x60;  // fix high nibble
    P.C = sum > 0xFF;
    A = uint8_t(sum);
    setNZ(A);


  }else{
    uint16_t sum = uint16_t(A)+uint16_t(val)+P.C;
    P.C = (sum > 0xFF);
    setNZ(sum);
    P.V = ~(A ^ val) & (A ^ uint8_t(sum)) & 0x80;
  }
}

void CPU::SBC(uint8_t val){
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


void CPU::CMP(uint8_t val){
  uint8_t T = A - val;
  setNZ(T);
  P.C = (T >= val);
}
uint8_t CPU::ROR(uint8_t val){
  bool oldC = P.C;
  P.C = (val & 0x01);
  val = (val >> 1) | (oldC ? 0x80 : 0);
  setNZ(val);
  return val;
}
uint8_t CPU::ROL(uint8_t val){
  bool oldC = P.C;
  P.C = (val & 0x80) != 0;
  val = (val << 1) | (oldC ? 1 : 0);
  setNZ(val);
  return val;
}
void CPU::LSR(uint16_t addr){
  uint8_t val = read8(addr);

  P.C = val & 0x01;
  val >>= 1;

  write8(addr, val);
  setNZ(val);
  P.N = 0;
}

// A
void CPU::ASL_A(){NI};
void CPU::LSR_A(){P.C = A & 0x01; A >>= 1; setNZ(A); P.N = 0;}; 
void CPU::ROL_A(){A = ROL(A);};
void CPU::ROR_A(){A = ROR(A);};
// imCPU::mediate
void CPU::ADC_IMM(){ADC(J);};
void CPU::AND_IMM(){A = A & J; setNZ(A);};
void CPU::CMP_IMM(){CMP(J);};
void CPU::CPX_IMM(){uint8_t T = X-J; setNZ(T); P.C = (T >= J);};
void CPU::CPY_IMM(){uint8_t T = Y-J; setNZ(T); P.C = (T >= J);};
void CPU::EOR_IMM(){A = A xor J; setNZ(A);};
void CPU::LDA_IMM(){ A = J; setNZ(A); };
void CPU::LDX_IMM(){ X = J; setNZ(X); };
void CPU::LDY_IMM(){ Y = J; setNZ(Y); };
void CPU::ORA_IMM(){A = A | J;};
void CPU::SBC_IMM(){SBC(J);};

//AsoCPU::ulute
uint16_t CPU::JK2u16(){ return uint16_t(J) | (uint16_t(K) << 8); }

void CPU::ADC_ABS(){ADC(read8(JK2u16()));};
void CPU::AND_ABS(){A = A & read8(JK2u16()); setNZ(A);};
void CPU::ASL_ABS(){NI};
void CPU::BIT_ABS(){NI};
void CPU::CMP_ABS(){CMP(read8(JK2u16()));};
void CPU::CPX_ABS(){uint8_t T = X-read8(JK2u16()); setNZ(T); P.C = (T >= read8(JK2u16()));};
void CPU::CPY_ABS(){uint8_t T = Y-read8(JK2u16()); setNZ(T); P.C = (T >= read8(JK2u16()));};
void CPU::DEC_ABS(){write8(JK2u16(), read8(JK2u16()) - 1);};
void CPU::EOR_ABS(){A = A xor read8(JK2u16()); setNZ(A);};
void CPU::INC_ABS(){write8(JK2u16(), read8(JK2u16()) + 1);};
void CPU::JMP_ABS(){PC = JK2u16();};
void CPU::JSR_ABS(){stackPush(uint8_t((PC & 0xFF00) >> 8)); stackPush(uint8_t(PC & 0xFF)); PC = JK2u16(); };
void CPU::LDA_ABS(){A = read8(JK2u16()); setNZ(A);};
void CPU::LDX_ABS(){X = read8(JK2u16()); setNZ(X);};
void CPU::LDY_ABS(){Y = read8(JK2u16()); setNZ(Y);};
void CPU::LSR_ABS(){LSR(read8(JK2u16()));};
void CPU::ORA_ABS(){A = A | read8(JK2u16()); setNZ(A);};
void CPU::ROL_ABS(){write8(JK2u16(),ROL(read8(JK2u16())));};
void CPU::ROR_ABS(){write8(JK2u16(),ROR(read8(JK2u16())));};
void CPU::SBC_ABS(){SBC(read8(JK2u16()));};
void CPU::STA_ABS(){write8(JK2u16(), A);};
void CPU::STX_ABS(){write8(JK2u16(), X);};
void CPU::STY_ABS(){write8(JK2u16(), Y);}; 

uint16_t CPU::JKX2u16(uint8_t XY){return JK2u16()+XY;};
// X CPU::indexed absoulute
void CPU::ADC_XiAbs(){ADC(read8(JKX2u16(X)));};
void CPU::AND_XiAbs(){A = A & read8(JKX2u16(X)); setNZ(A);};
void CPU::ASL_XiAbs(){NI};
void CPU::CMP_XiAbs(){CMP(read8(JKX2u16(X)));};
void CPU::DEC_XiAbs(){write8(JKX2u16(Y), read8(JKX2u16(Y)) - 1);};
void CPU::EOR_XiAbs(){A = A xor read8(JKX2u16(X)); setNZ(A);};
void CPU::INC_XiAbs(){write8(JKX2u16(X), read8(JKX2u16(X)) + 1);};
void CPU::LDA_XiAbs(){A = read8(JKX2u16(X)); setNZ(A);};
void CPU::LDY_XiAbs(){Y = read8(JKX2u16(X)); setNZ(Y);};
void CPU::LSR_XiAbs(){LSR(read8(JKX2u16(X)));};
void CPU::ORA_XiAbs(){A = A | read8(JKX2u16(X)); setNZ(A);};
void CPU::ROL_XiAbs(){write8(JKX2u16(X),ROL(read8(JKX2u16(X))));};
void CPU::ROR_XiAbs(){write8(JKX2u16(X),ROR(read8(JKX2u16(X))));};
void CPU::SBC_XiAbs(){SBC(read8(JKX2u16(X)));};
void CPU::STA_XiAbs(){write8(JKX2u16(X), A);};

// Y CPU::indexed absoulute
void CPU::ADC_YiAbs(){ADC(read8(JKX2u16(Y)));};
void CPU::AND_YiAbs(){A = A & read8(JKX2u16(Y)); setNZ(A);};
void CPU::ASL_YiAbs(){NI};
void CPU::CMP_YiAbs(){CMP(read8(JKX2u16(Y)));};
void CPU::EOR_YiAbs(){A = A xor read8(JKX2u16(Y)); setNZ(A);};
void CPU::LDA_YiAbs(){A = read8(JKX2u16(Y)); setNZ(A);};
void CPU::LDX_YiAbs(){X = read8(JKX2u16(Y)); setNZ(X);};
void CPU::LDY_YiAbs(){Y = read8(JKX2u16(Y)); setNZ(Y);};
void CPU::ORA_YiAbs(){A = A | read8(JKX2u16(Y)); setNZ(A);};
void CPU::SBC_YiAbs(){SBC(read8(JKX2u16(Y)));};
void CPU::STA_YiAbs(){write8(JKX2u16(Y), A);};

// abCPU::soulute indirect
void CPU::JMP_AbsInd(){uint16_t effective = JK2u16();
                       PC = read8(effective) | (uint16_t(read8(effective+1)) << 8);};

void CPU::ADC_ZERO(){ADC(read8(J));};
void CPU::AND_ZERO(){A = A & read8(J); setNZ(A);};
void CPU::ASL_ZERO(){NI};
void CPU::BIT_ZERO(){NI};
void CPU::CMP_ZERO(){CMP(read8(J));};
void CPU::CPX_ZERO(){uint8_t T = Y-read8(J); setNZ(T); P.C = (T >= read8(J));};
void CPU::CPY_ZERO(){uint8_t T = Y-read8(J); setNZ(T); P.C = (T >= read8(J));};
void CPU::DEC_ZERO(){write8(J, read8(J) - 1); setNZ(read8(J));};
void CPU::EOR_ZERO(){A = A xor read8(J); setNZ(A);};
void CPU::INC_ZERO(){write8(J, read8(J) + 1); setNZ(read8(J));};
void CPU::LDA_ZERO(){A = read8(J); setNZ(A);};
void CPU::LDX_ZERO(){X = read8(J); setNZ(X);};
void CPU::LDY_ZERO(){Y = read8(J); setNZ(Y);};
void CPU::LSR_ZERO(){LSR(read8(J));};
void CPU::ORA_ZERO(){A = A | read8(J); setNZ(A);};
void CPU::ROL_ZERO(){write8(J,ROL(read8(J)));};
void CPU::ROR_ZERO(){write8(J,ROR(read8(J)));};
void CPU::SBC_ZERO(){SBC(read8(J));};
void CPU::STA_ZERO(){write8(J, A);};
void CPU::STX_ZERO(){write8(J, X);};
void CPU::STY_ZERO(){write8(J, Y);};

//ZerCPU::opage X
void CPU::ADC_ZPX(){ADC(read8(J+X));};
void CPU::AND_ZPX(){A = A & read8(J+X); setNZ(A);};
void CPU::ASL_ZPX(){NI};
void CPU::CMP_ZPX(){CMP(read8(J+X));};
void CPU::DEC_ZPX(){write8(J+X, read8(J+X) - 1);};
void CPU::EOR_ZPX(){A = A xor read8(J+X); setNZ(A);};
void CPU::INC_ZPX(){write8(J+X, read8(J+X) + 1);};
void CPU::LDA_ZPX(){A = read8(J+X); setNZ(A);};
void CPU::LDY_ZPX(){Y = read8(J+X); setNZ(Y);};
void CPU::LSR_ZPX(){LSR(read8(J+X));};
void CPU::ORA_ZPX(){A = A | read8(J+X); setNZ(A);};
void CPU::ROL_ZPX(){write8(J+X,ROL(read8(J+X)));};
void CPU::ROR_ZPX(){write8(J+X,ROR(read8(J+X)));};
void CPU::SBC_ZPX(){SBC(read8(J+X));};
void CPU::STA_ZPX(){write8(J+X, A);};
void CPU::STY_ZPX(){write8(J+X, Y);};

//ZerCPU::opage Y
void CPU::LDX_ZPY(){NI};
void CPU::STX_ZPY(){NI};

uint16_t CPU::XIndexZPIND(){
  return read8(J+X) | (read8(J+X+1) << 8);
}
// ZeCPU::ropage X indirect
void CPU::ADC_XZPI(){ADC(read8(XIndexZPIND()));};
void CPU::AND_XZPI(){A = A & read8(XIndexZPIND()); setNZ(A);};
void CPU::CMP_XZPI(){CMP(read8(XIndexZPIND()));};
void CPU::EOR_XZPI(){A = A xor read8(XIndexZPIND()); setNZ(A);};
void CPU::LDA_XZPI(){A = read8(XIndexZPIND()); setNZ(A);};
void CPU::ORA_XZPI(){NI};
void CPU::SBC_XZPI(){SBC(read8(XIndexZPIND()));};
void CPU::STA_XZPI(){NI};
uint16_t CPU::ZPINDYIndex(){
  return uint16_t(read8(J)) + ((uint16_t)read8(J+1) << 8) + Y;
}
//zerCPU::opage Y indirect
void CPU::ADC_ZPIY(){ADC(read8(ZPINDYIndex()));};
void CPU::AND_ZPIY(){A = A & read8(ZPINDYIndex()); setNZ(A);};
void CPU::CMP_ZPIY(){CMP(read8(ZPINDYIndex()));};
void CPU::EOR_ZPIY(){A = A xor read8(ZPINDYIndex()); setNZ(A);};
void CPU::LDA_ZPIY(){A = read8(ZPINDYIndex()); setNZ(A);};
void CPU::ORA_ZPIY(){A = A | read8(ZPINDYIndex()); setNZ(A);};
void CPU::SBC_ZPIY(){SBC(read8(ZPINDYIndex()));};
void CPU::STA_ZPIY(){write8(ZPINDYIndex(), A);};

void CPU::jumpRelative(bool jump){
  if (jump) {
    PC = uint16_t(static_cast<int8_t>(J) + PC);
  } 
};
//RelCPU::ative
void CPU::BCC_REL(){jumpRelative(!P.C);};
void CPU::BCS_REL(){jumpRelative(P.C);};
void CPU::BEQ_REL(){jumpRelative(P.Z);};
void CPU::BMI_REL(){jumpRelative(P.N);};
void CPU::BNE_REL(){jumpRelative(!P.Z);};
void CPU::BPL_REL(){jumpRelative(!P.N);};
void CPU::BVC_REL(){jumpRelative(!P.V);};
void CPU::BVS_REL(){jumpRelative(P.V);};



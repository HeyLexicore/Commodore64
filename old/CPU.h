#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>

const int opcodeLen[256]{
//0 1 2 3 4 5 6 7 8 9 A B C D E F
  1,2,1,2,2,2,2,2,1,2,1,2,3,3,3,3, // 0-
  2,2,1,2,2,2,2,2,1,3,1,3,3,3,3,3, // 1-
  3,2,1,2,2,2,2,2,1,2,1,2,3,3,3,3, // 2-
  2,2,1,2,2,2,2,2,1,3,1,3,3,3,3,3, // 3-
  1,2,1,2,2,2,2,2,1,2,1,2,3,3,3,3, // 4-
  2,2,1,2,2,2,2,2,1,3,1,3,3,3,3,3, // 5-
  1,2,1,2,2,2,2,2,1,2,1,2,3,3,3,3, // 6-
  2,2,1,2,2,2,2,2,1,3,1,3,3,3,3,3, // 7-
  2,2,2,2,2,2,2,2,1,2,1,2,3,3,3,3, // 8-
  2,2,1,2,2,2,2,2,1,3,1,3,3,3,3,3, // 9-
  2,2,2,2,2,2,2,2,1,2,1,2,3,3,3,3, // A-
  2,2,1,2,2,2,2,2,1,3,1,3,3,3,3,3, // B-
  2,2,2,2,2,2,2,2,1,2,1,2,3,3,3,3, // C-
  2,2,1,2,2,2,2,2,1,3,1,3,3,3,3,3, // D-
  2,2,2,2,2,2,2,2,1,2,1,2,3,3,3,3, // E-
  2,2,1,2,2,2,2,2,1,3,1,3,3,3,3,3  // F-
};

struct CPU{
  std::array<uint8_t, 0x10000> memory;
  
  uint8_t A{}, X{}, Y{}, SP{0xFD};
  uint16_t PC{};

  struct {
    bool C,Z,I,D,B,EX,V,N;
    uint8_t pack() const {
      return C | (Z<<1) | (I<<2) | (D<<3) | (B<<4) | (EX<<5) | (V<<6) | (N<<7);
    }
    void unpack(uint8_t v){
      C = ((v & 0x1) != 0);
      Z = (((v >> 1) & 0x1) != 0);
      I = (((v >> 2) & 0x1) != 0);
      D = (((v >> 3) & 0x1) != 0);
      B = (((v >> 4) & 0x1) != 0);
     EX = (((v >> 5)& 0x1) != 0);
      V = (((v >> 6) & 0x1) != 0);
      N = (((v >> 7) & 0x1) != 0);
    }
  } P;
  
  uint8_t read8(const uint16_t addr);
  uint16_t read16LE(const uint16_t addr);
  void write8(const uint16_t addr, uint8_t val);
  void load_file(const char* filename, uint16_t offset);
  uint8_t fetch();
  void advance();
  void print_opc(uint16_t addr);
  void print_reg();
  std::string opcodeRepr[256];
  void setNZ(uint8_t val);
  void init_opcodeRepr();


  uint8_t J,K;
  void setJK(){ J = read8(PC+1); K = read8(PC+2);};
  typedef void (CPU::*Exec)();
  
  Exec opcodeTable[256];
  
  void execute(uint8_t opcode);
  void stackPush(uint8_t val);
  uint8_t stackPull();


  uint16_t JK2u16();
  uint16_t JKX2u16(uint8_t XY);
  uint16_t XIndexZPIND();
  uint16_t ZPINDYIndex();
  void jumpRelative(bool jump);
  
  void ADC(uint8_t val);
  void SBC(uint8_t val);
  void CMP(uint8_t val);
  uint8_t ROR(uint8_t val);
  uint8_t ROL(uint8_t val);
  void LSR(uint16_t addr);

  CPU(){
    init_opcodeRepr();

    for (int i = 0; i < 256; i++){
      opcodeTable[i] = &CPU::NOP;
    }
  
    opcodeTable[0x00]=&CPU::BRK;
    opcodeTable[0x01]=&CPU::ORA_XZPI; 
    opcodeTable[0x05]=&CPU::ORA_ZERO; 
    opcodeTable[0x06]=&CPU::ASL_ZERO; 
    opcodeTable[0x08]=&CPU::PHP;
    opcodeTable[0x09]=&CPU::ORA_IMM; 
    opcodeTable[0x0A]=&CPU::ASL_A; 
    opcodeTable[0x0D]=&CPU::ORA_ABS; 
    opcodeTable[0x0E]=&CPU::ASL_ABS; 
    opcodeTable[0x10]=&CPU::BPL_REL; 
    opcodeTable[0x11]=&CPU::ORA_ZPIY; 
    opcodeTable[0x15]=&CPU::ORA_XZPI; 
    opcodeTable[0x16]=&CPU::ASL_ZPX; 
    opcodeTable[0x18]=&CPU::CLC;
    opcodeTable[0x19]=&CPU::ORA_YiAbs; 
    opcodeTable[0x1D]=&CPU::ORA_XiAbs; 
    opcodeTable[0x1E]=&CPU::ASL_XiAbs; 
    opcodeTable[0x20]=&CPU::JSR_ABS; 
    opcodeTable[0x21]=&CPU::AND_XZPI; 
    opcodeTable[0x24]=&CPU::BIT_ZERO; 
    opcodeTable[0x25]=&CPU::AND_ZERO; 
    opcodeTable[0x26]=&CPU::ROL_ZERO; 
    opcodeTable[0x28]=&CPU::PLP;
    opcodeTable[0x29]=&CPU::AND_IMM; 
    opcodeTable[0x2A]=&CPU::ROL_A; 
    opcodeTable[0x2C]=&CPU::BIT_ABS; 
    opcodeTable[0x2D]=&CPU::AND_ABS; 
    opcodeTable[0x2E]=&CPU::ROL_ABS; 
    opcodeTable[0x30]=&CPU::BMI_REL; 
    opcodeTable[0x31]=&CPU::AND_ZPIY; 
    opcodeTable[0x35]=&CPU::AND_XZPI; 
    opcodeTable[0x36]=&CPU::ROL_ZPX; 
    opcodeTable[0x38]=&CPU::SEC;
    opcodeTable[0x39]=&CPU::AND_YiAbs; 
    opcodeTable[0x3D]=&CPU::AND_XiAbs; 
    opcodeTable[0x3E]=&CPU::ROL_XiAbs; 
    opcodeTable[0x40]=&CPU::RTI;
    opcodeTable[0x41]=&CPU::EOR_XZPI; 
    opcodeTable[0x45]=&CPU::EOR_ZERO; 
    opcodeTable[0x46]=&CPU::LSR_ZERO; 
    opcodeTable[0x48]=&CPU::PHA;
    opcodeTable[0x49]=&CPU::EOR_IMM; 
    opcodeTable[0x4A]=&CPU::LSR_A; 
    opcodeTable[0x4C]=&CPU::JMP_ABS; 
    opcodeTable[0x4D]=&CPU::EOR_ABS; 
    opcodeTable[0x4E]=&CPU::LSR_ABS; 
    opcodeTable[0x50]=&CPU::BVC_REL; 
    opcodeTable[0x51]=&CPU::EOR_ZPIY; 
    opcodeTable[0x55]=&CPU::EOR_XZPI; 
    opcodeTable[0x56]=&CPU::LSR_ZPX; 
    opcodeTable[0x58]=&CPU::CLI;
    opcodeTable[0x59]=&CPU::EOR_YiAbs; 
    opcodeTable[0x5D]=&CPU::EOR_XiAbs; 
    opcodeTable[0x5E]=&CPU::LSR_XiAbs; 
    opcodeTable[0x60]=&CPU::RTS;
    opcodeTable[0x61]=&CPU::ADC_XZPI; 
    opcodeTable[0x65]=&CPU::ADC_ZERO; 
    opcodeTable[0x66]=&CPU::ROR_ZERO; 
    opcodeTable[0x68]=&CPU::PLA;
    opcodeTable[0x69]=&CPU::ADC_IMM; 
    opcodeTable[0x6A]=&CPU::ROR_A; 
    opcodeTable[0x6C]=&CPU::JMP_AbsInd; 
    opcodeTable[0x6D]=&CPU::ADC_ABS; 
    opcodeTable[0x6E]=&CPU::ROR_ABS; 
    opcodeTable[0x70]=&CPU::BVS_REL; 
    opcodeTable[0x71]=&CPU::ADC_ZPIY; 
    opcodeTable[0x75]=&CPU::ADC_XZPI; 
    opcodeTable[0x76]=&CPU::ROR_ZPX; 
    opcodeTable[0x78]=&CPU::SEI;
    opcodeTable[0x79]=&CPU::ADC_YiAbs; 
    opcodeTable[0x7D]=&CPU::ADC_XiAbs; 
    opcodeTable[0x7E]=&CPU::ROR_XiAbs; 
    opcodeTable[0x81]=&CPU::STA_XZPI; 
    opcodeTable[0x84]=&CPU::STY_ZERO; 
    opcodeTable[0x85]=&CPU::STA_ZERO; 
    opcodeTable[0x86]=&CPU::STX_ZERO; 
    opcodeTable[0x88]=&CPU::DEY;
    opcodeTable[0x8A]=&CPU::TXA;
    opcodeTable[0x8C]=&CPU::STY_ABS; 
    opcodeTable[0x8D]=&CPU::STA_ABS; 
    opcodeTable[0x8E]=&CPU::STX_ABS; 
    opcodeTable[0x90]=&CPU::BCC_REL; 
    opcodeTable[0x91]=&CPU::STA_ZPIY; 
    opcodeTable[0x94]=&CPU::STY_ZPX; 
    opcodeTable[0x95]=&CPU::STA_ZPX; 
    opcodeTable[0x96]=&CPU::STX_ZPY; 
    opcodeTable[0x98]=&CPU::TYA;
    opcodeTable[0x99]=&CPU::STA_YiAbs; 
    opcodeTable[0x9A]=&CPU::TXS;
    opcodeTable[0x9D]=&CPU::STA_XiAbs; 
    opcodeTable[0xA0]=&CPU::LDY_IMM; 
    opcodeTable[0xA1]=&CPU::LDA_XZPI; 
    opcodeTable[0xA2]=&CPU::LDX_IMM; 
    opcodeTable[0xA4]=&CPU::LDY_ZERO; 
    opcodeTable[0xA5]=&CPU::LDA_ZERO; 
    opcodeTable[0xA6]=&CPU::LDX_ZERO; 
    opcodeTable[0xA8]=&CPU::TAY;
    opcodeTable[0xA9]=&CPU::LDA_IMM; 
    opcodeTable[0xAA]=&CPU::TAX;
    opcodeTable[0xAC]=&CPU::LDY_ABS; 
    opcodeTable[0xAD]=&CPU::LDA_ABS; 
    opcodeTable[0xAE]=&CPU::LDX_ABS; 
    opcodeTable[0xB0]=&CPU::BCS_REL; 
    opcodeTable[0xB1]=&CPU::LDA_ZPIY; 
    opcodeTable[0xB4]=&CPU::LDY_ZPX; 
    opcodeTable[0xB5]=&CPU::LDA_XZPI; 
    opcodeTable[0xB6]=&CPU::LDX_ZPY; 
    opcodeTable[0xB8]=&CPU::CLV;
    opcodeTable[0xB9]=&CPU::LDA_YiAbs; 
    opcodeTable[0xBA]=&CPU::TSX;
    opcodeTable[0xBC]=&CPU::LDY_XiAbs; 
    opcodeTable[0xBD]=&CPU::LDA_XiAbs; 
    opcodeTable[0xBE]=&CPU::LDX_YiAbs; 
    opcodeTable[0xC0]=&CPU::CPY_IMM; 
    opcodeTable[0xC1]=&CPU::CMP_XZPI; 
    opcodeTable[0xC4]=&CPU::CPY_ZERO; 
    opcodeTable[0xC5]=&CPU::CMP_ZERO; 
    opcodeTable[0xC6]=&CPU::DEC_ZERO; 
    opcodeTable[0xC8]=&CPU::INY;
    opcodeTable[0xC9]=&CPU::CMP_IMM; 
    opcodeTable[0xCA]=&CPU::DEX;
    opcodeTable[0xCC]=&CPU::CPY_ABS; 
    opcodeTable[0xCD]=&CPU::CMP_ABS; 
    opcodeTable[0xCE]=&CPU::DEC_ABS; 
    opcodeTable[0xD0]=&CPU::BNE_REL; 
    opcodeTable[0xD1]=&CPU::CMP_ZPIY; 
    opcodeTable[0xD5]=&CPU::CMP_XZPI; 
    opcodeTable[0xD6]=&CPU::DEC_ZPX; 
    opcodeTable[0xD8]=&CPU::CLD;
    opcodeTable[0xD9]=&CPU::CMP_YiAbs; 
    opcodeTable[0xDD]=&CPU::CMP_XiAbs; 
    opcodeTable[0xDE]=&CPU::DEC_XiAbs; 
    opcodeTable[0xE0]=&CPU::CPX_IMM; 
    opcodeTable[0xE1]=&CPU::SBC_XZPI; 
    opcodeTable[0xE4]=&CPU::CPX_ZERO; 
    opcodeTable[0xE5]=&CPU::SBC_ZERO; 
    opcodeTable[0xE6]=&CPU::INC_ZERO; 
    opcodeTable[0xE8]=&CPU::INX;
    opcodeTable[0xE9]=&CPU::SBC_IMM; 
    opcodeTable[0xEA]=&CPU::NOP;
    opcodeTable[0xEC]=&CPU::CPX_ABS; 
    opcodeTable[0xED]=&CPU::SBC_ABS; 
    opcodeTable[0xEE]=&CPU::INC_ABS; 
    opcodeTable[0xF0]=&CPU::BEQ_REL; 
    opcodeTable[0xF1]=&CPU::SBC_ZPIY; 
    opcodeTable[0xF5]=&CPU::SBC_XZPI; 
    opcodeTable[0xF6]=&CPU::INC_ZPX; 
    opcodeTable[0xF8]=&CPU::SED;
    opcodeTable[0xF9]=&CPU::SBC_YiAbs; 
    opcodeTable[0xFD]=&CPU::SBC_XiAbs; 
    opcodeTable[0xFE]=&CPU::INC_XiAbs; 
  }


  // implied
  void BRK(); // $00
  void CLC(); // $18
  void CLD(); // $D8
  void CLI(); // $58
  void CLV(); // $B8
  void DEX(); // $CA
  void DEY(); // $88
  void INX(); // $E8
  void INY(); // $C8
  void NOP(); // $EA
  void PHA(); // $48
  void PHP(); // $08
  void PLA(); // $68
  void PLP(); // $28
  void RTI(); // $40
  void RTS(); // $60
  void SEC(); // $38
  void SED(); // $F8
  void SEI(); // $78
  void TAX(); // $AA
  void TAY(); // $A8
  void TSX(); // $BA
  void TXA(); // $8A
  void TXS(); // $9A
  void TYA(); // $98

  // A
  void ASL_A();
  void LSR_A(); 
  void ROL_A();
  void ROR_A();
  // immediate
  void ADC_IMM();
  void AND_IMM();
  void CMP_IMM();
  void CPX_IMM();
  void CPY_IMM();
  void EOR_IMM();
  void LDA_IMM();
  void LDX_IMM();
  void LDY_IMM();
  void ORA_IMM();
  void SBC_IMM();
  
  //Asoulute
  void ADC_ABS();
  void AND_ABS();
  void ASL_ABS();
  void BIT_ABS();
  void CMP_ABS();
  void CPX_ABS();
  void CPY_ABS();
  void DEC_ABS();
  void EOR_ABS();
  void INC_ABS();
  void JMP_ABS();
  void JSR_ABS();
  void LDA_ABS();
  void LDX_ABS();
  void LDY_ABS();
  void LSR_ABS();
  void ORA_ABS();
  void ROL_ABS();
  void ROR_ABS();
  void SBC_ABS();
  void STA_ABS();
  void STX_ABS();
  void STY_ABS(); 

  // X indexed absoulute
  void ADC_XiAbs();
  void AND_XiAbs();
  void ASL_XiAbs();
  void CMP_XiAbs();
  void DEC_XiAbs();
  void EOR_XiAbs();
  void INC_XiAbs();
  void LDA_XiAbs();
  void LDY_XiAbs();
  void LSR_XiAbs();
  void ORA_XiAbs();
  void ROL_XiAbs();
  void ROR_XiAbs();
  void SBC_XiAbs();
  void STA_XiAbs();
  
  // Y indexed abslute
  void ADC_YiAbs();
  void AND_YiAbs();
  void ASL_YiAbs();
  void CMP_YiAbs();
  void EOR_YiAbs();
  void LDA_YiAbs();
  void LDX_YiAbs();
  void LDY_YiAbs();
  void ORA_YiAbs();
  void SBC_YiAbs();
  void STA_YiAbs();
  
  // absoulute indirect
  void JMP_AbsInd();
 
  void ADC_ZERO();
  void AND_ZERO();
  void ASL_ZERO();
  void BIT_ZERO();
  void CMP_ZERO();
  void CPX_ZERO();
  void CPY_ZERO();
  void DEC_ZERO();
  void EOR_ZERO();
  void INC_ZERO();
  void LDA_ZERO();
  void LDX_ZERO();
  void LDY_ZERO();
  void LSR_ZERO();
  void ORA_ZERO();
  void ROL_ZERO();
  void ROR_ZERO();
  void SBC_ZERO();
  void STA_ZERO();
  void STX_ZERO();
  void STY_ZERO();

  //Zeropage X
  void ADC_ZPX();
  void AND_ZPX();
  void ASL_ZPX();
  void CMP_ZPX();
  void DEC_ZPX();
  void EOR_ZPX();
  void INC_ZPX();
  void LDA_ZPX();
  void LDY_ZPX();
  void LSR_ZPX();
  void ORA_ZPX();
  void ROL_ZPX();
  void ROR_ZPX();
  void SBC_ZPX();
  void STA_ZPX();
  void STY_ZPX();
  
  //Zeropage Y
  void LDX_ZPY();
  void STX_ZPY();

  // Zeropage X indirect
  void ADC_XZPI();
  void AND_XZPI();
  void CMP_XZPI();
  void EOR_XZPI();
  void LDA_XZPI();
  void ORA_XZPI();
  void SBC_XZPI();
  void STA_XZPI();
  
  //zeropage Y inrect
  void ADC_ZPIY();
  void AND_ZPIY();
  void CMP_ZPIY();
  void EOR_ZPIY();
  void LDA_ZPIY();
  void ORA_ZPIY();
  void SBC_ZPIY();
  void STA_ZPIY();
  

  //Relative
  void BCC_REL();
  void BCS_REL();
  void BEQ_REL();
  void BMI_REL();
  void BNE_REL();
  void BPL_REL();
  void BVC_REL();
  void BVS_REL();
  

};



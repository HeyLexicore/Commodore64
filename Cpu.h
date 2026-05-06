#include "Bus.h"
#include "common.h"
#include <cstdint>

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

const int opcodeCycles[256]{

	7,6,2,8,3,3,5,5,3,2,2,2,4,4,6,6,
	2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7,
	6,6,2,8,3,3,5,5,4,2,2,2,4,4,6,6,
	2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7,
	6,6,2,8,3,3,5,5,3,2,2,2,3,4,6,6,
	2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7,
	6,6,2,8,3,3,5,5,4,2,2,2,5,4,6,6,
	2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7,
	2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,
	2,6,2,6,4,4,4,4,2,5,2,5,5,5,5,5,
	2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,
	2,5,2,5,4,4,4,4,2,4,2,4,4,4,4,4,
	2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,
	2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7,
	2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,
	2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7
};
class Cpu{
  private:
    
    uint8_t A{}, X{}, Y{}, SP{0xFD}, J{}, K{};
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

    Bus* bus;

    typedef void (Cpu::*Exec)(int);

    
    Exec opcodeExecution[56];
    
    int opcodeCode[256],opcodeMode[256];
    std::string opcodeMnem[256];

    uint8_t valueJK(int);
    uint16_t addressJK(int);
    void setJK(int);
    uint16_t combineJK();

    void advance_PC();
    uint8_t fetch();
    void execute(uint8_t);

    void setNZ(uint8_t);
    void jumpRelative(bool);
     
    void stackPush(uint8_t);
    uint8_t stackPull();

    void ADC(int);
    void AND(int);
    void ASL(int);
    void BCC(int);
    void BCS(int);
    void BEQ(int);
    void BIT(int);
    void BMI(int);
    void BNE(int);
    void BPL(int);
    void BRK(int);
    void BVC(int);
    void BVS(int);
    void CLC(int);
    void CLD(int);
    void CLI(int);
    void CLV(int);
    void CMP(int);
    void CPX(int);
    void CPY(int);
    void DEC(int);
    void DEX(int);
    void DEY(int);
    void EOR(int);
    void INC(int);
    void INX(int);
    void INY(int);
    void JMP(int);
    void JSR(int);
    void LDA(int);
    void LDX(int);
    void LDY(int);
    void LSR(int);
    void NOP(int);
    void ORA(int);
    void PHA(int);
    void PHP(int);
    void PLA(int);
    void PLP(int);
    void ROL(int);
    void ROR(int);
    void RTI(int);
    void RTS(int);
    void SBC(int);
    void SEC(int);
    void SED(int);
    void SEI(int);
    void STA(int);
    void STX(int);
    void STY(int);
    void TAX(int);
    void TAY(int);
    void TSX(int);
    void TXA(int);
    void TXS(int);
    void TYA(int);

  public:
    Cpu(){};
    Cpu(Bus*);

    void setupExecs();
    void setupOpcodes();
    void setupReturnVector();
    int cycle(settings);

};

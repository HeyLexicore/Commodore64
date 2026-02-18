#include <csignal>
#include <cstdint>
#include <cstdio>
#include "CPU.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
void print_status(CPU &cpu){
  printf("%s%s%s%s%s%s%s%s",
        cpu.P.N?"N":"-",
        cpu.P.V?"V":"-",
        cpu.P.EX?"-":"-",
        cpu.P.B?"B":"-",
        cpu.P.D?"D":"-",
        cpu.P.I?"I":"-",
        cpu.P.Z?"Z":"-",
        cpu.P.C?"C":"-");
  // what in the magic unicde is this
  printf(" \x1B[0;34m%04X \x1B[35m%04X \x1B[36mA %02X \x1B[32mX %02X \x1B[31mY %02X \x1B[34m%02X ",cpu.SP,cpu.PC, cpu.A, cpu.X, cpu.Y, cpu.fetch());
  cpu.print_opc(cpu.PC);
  for (int i = 1; i < opcodeLen[cpu.fetch()]; i++){
    printf("%02X ",cpu.read8(cpu.PC+i));
  }
  printf("\x1B[0m\n"); 
}


void renderScreen(CPU &cpu) {
    const uint16_t SCREEN_START = 0x0400; // start of screen RAM
    const int WIDTH = 40;
    const int HEIGHT = 25;

    for (int row = 0; row < HEIGHT; ++row) {
        for (int col = 0; col < WIDTH; ++col) {
            uint16_t addr = SCREEN_START + row * WIDTH + col;
            uint8_t charCode = cpu.read8(addr);

            // Print ASCII printable characters if possible
            if (charCode >= 32 && charCode <= 126) {
                std::cout << static_cast<char>(charCode);
            } else {
                std::cout << '.'; // placeholder for non-printable characters
            }
        }
        std::cout << '\n';
    }
  }

void print_ram(CPU &cpu, int start, int end) {
    for (int i = start; i < end; i += 16) {
        std::cerr << std::hex << std::uppercase
                  << std::setw(4) << std::setfill('0') << i << " ";

        for (int x = 0; x < 16; x++) {
            std::cerr << std::setw(2) << std::setfill('0')
                      << static_cast<int>(cpu.read8(i + x)) << " ";
        }
        std::cerr << '\n';
    }
}

bool stop_requested = false;
void handle_sigint(int signal){
  if (signal == SIGINT){
    stop_requested = true;
  }
}

int main (int argc, char *argv[]) {
  CPU cpu;
  
  switch (argc){
    case 2:
      cpu.load_file(argv[1],0xE000);
      break;
    case 3:
      cpu.load_file(argv[1],0xE000);
      cpu.load_file(argv[2],0xA000);
      break;
    default:
      printf("Wrong amount of arguments");
      return 1;
  }
  // --------------------------
  // uint8_t cbm80[5] = {0xC3,0xC2,0xCD,0x38,0x30};
  // for (int i = 0; i< 5; i++){
  //  cpu.write8(0x8004+i, cbm80[i]);
  //}
  // --------------------------
  cpu.write8(0xD011, 0x1B);
  cpu.write8(0xD021, 0x06);
  cpu.write8(0xD020, 0x06);

  cpu.write8(0xDC00, 0xFF);
  cpu.write8(0xDC01, 0xFF);
  
  cpu.write8(0xDD00, 0xFF);
  cpu.write8(0xDD01, 0xFF);

  cpu.PC = cpu.read16LE(0xFFFC);

  
  long int cycle_count = 1;
  int raster_line = 0;

  uint8_t opcode = 1;
  while (opcode != 0x00){ //repeat until opcode BRK
    opcode = cpu.fetch();
//    if (cycle_count > 21055713){
//      print_ram(cpu, 0x0400, 0x0400+40*20);
//   }
    cpu.setJK();
    print_status(cpu);
    cpu.advance();
    cpu.execute(opcode);

    cpu.memory[0xD012] = uint8_t(raster_line);
    cpu.memory[0xD011] = (cpu.read8(0xD011) & 0x7F) | ((raster_line & 0x100) >> 1);
    
    if (cycle_count % 63 == 0){ // implement cycle counting here lmao
      raster_line++;
      if (raster_line > 311) raster_line = 0;
    }
    
    cycle_count += 4;
    
    if (cycle_count  > 200){
      abort();
    }

    
  }


  return 0;
}


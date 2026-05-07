#include "Bus.h"
#include "Cpu.h"
#include "Vic.h"
#include "common.h"
#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>

const int CLOCKSPEED = 1000000; // 1 Mhz

int main (int argc, char *argv[]) {
  
  if (argc < 3){
    printf("Too little args\n");
    printf("Usage: <command> <kernal> <basic> [-v Print Verbose] [ -V Print Superverbose ] [ -o Print Opcodes] [ -f Fast ]\n");
    return 1;
  } else if (argc >= 3) {
  }
  
  
  settings set;
  for (int i = 3; i < argc; i++){
    if (strcmp(argv[i],"-v")==0) set.printVerbose = true;
    if (strcmp(argv[i],"-V")==0) {set.printSuperverbose = true; set.printVerbose = true;};
    if (strcmp(argv[i],"-o")==0) set.printOpcodes = true;
    if (strcmp(argv[i],"-f")==0) set.fast = true;
  }

  Bus bus(set);
  Vic vic;
  bus.setupVic(&vic);
   
  bus.writeKernal(argv[1]);
  bus.writeBasic(argv[2]);

  bus.defaults();

 
  Cpu cpu(&bus);
  
  cpu.setupOpcodes();
  cpu.setupExecs();
  cpu.setupReturnVector();
  auto t0 = std::chrono::high_resolution_clock::now();
  int cycles = 0; 
  while(true){
    if (!set.fast){
    t0 = std::chrono::high_resolution_clock::now();
    }
    
    cycles = cpu.cycle(set);
    vic.advance_rasterline(cycles);

    if (!set.fast){
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000/CLOCKSPEED) - duration);
    }
  }

  return 0;
}

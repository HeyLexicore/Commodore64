#include "Bus.h"
#include <cstdio>



int main (int argc, char *argv[]) {
  Bus bus;
  bus.defaults();
  
  if (argc > 3){
    printf("Too many args\n");
    return 1;
  } else if (argc < 3){
    printf("Too little args\n");
    return 1;
  } else if (argc == 3) {
    bus.writeKernal(argv[1]);
    bus.writeBasic(argv[2]);
  }

  bus.write8(0x0010, 0x10);
  //bus.write8(0xB010, 0x11);
  bus.write8(0xC010, 0x12);
  bus.write8(0xD010, 0x13);
  //bus.write8(0xE010, 0x14);

  printf("%02X\n",bus.read8(0x0010));
  printf("%02X\n",bus.read8(0xB010));
  printf("%02X\n",bus.read8(0xC010));
  printf("%02X\n",bus.read8(0xD010));
  printf("%02X\n",bus.read8(0xE010));

  //printf("%i",bus.read8(0xC000));

  return 0;
}

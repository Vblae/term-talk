#include <stdio.h> 
#include "tserv/default.h"

int main(int argc, char** argv) {
  printf("defaults:\n  ip: %s\n  port: %d\n", DEFAULT_ADDR, DEFAULT_PORT);
  return 0;
}

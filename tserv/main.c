#include <stdio.h> 
#include "tserv/default.h"
#include "config/config.h"

int main(int argc, char** argv) {

  config_s* conf = load_config(argv[1]);
  printf("p: %p\n", conf);
  return 0;
}

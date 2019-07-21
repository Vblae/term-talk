#include <stdio.h> 

#include "tserv/default.h"
#include "config/config.h"
#include "util/vector.h"

int main(int argc, char** argv) {

  config_s* conf = load_config(argv[1]);
  printf("p: %p\n", conf);

  vector_s* vector = vector_int_create(0);

  for(int i = 0; i < 10; i++) {
    vector_push(vector, &i);
  }

  for(int i = 0; i < vector->len; i++) {
    int* val = (int*) vector_get(vector, i);
    printf("%p: %d\n", val, *val);
  }

  printf("\n");
  vector_free(vector);
  return 0;
}

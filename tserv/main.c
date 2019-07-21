#include <stdio.h> 

#include "tserv/default.h"
#include "config/config.h"
#include "util/vector.h"

int main(int argc, char** argv) {

  config_s* conf = load_config(argv[1]);
  printf("p: %p\n", conf);

  vector_s* vector = vector_of_string_create(0);
  char* test[] = {
    "str 1",
    "str 2",
    "str 3"
  };
  for(int i = 0; i < 3; i++) {
    vector_push(vector, &test[i]);
  }

  for(int i = 0; i < vector->len; i++) {
    char** val = (char**) vector_get(vector, i);
    (*val)[0] = 'T';
    printf("og:%p->%s new:%p->%s\n", test[i], test[i], *val, *val);
  }

  printf("\n");
  vector_free(vector);
  return 0;
}

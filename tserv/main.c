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
    (char*) 0,
    "str 3"
  };
  for(int i = 0; i < 4; i++) {
    vector_push(vector, &test[i]);
  }

  for(int i = 0; i < vector->len; i++) {
    char** val = (char**) vector_get(vector, i);
    printf("og:%p->%s new:%p->%s\n", test[i], test[i], *val, *val);
  }

  printf("top: %s\n", *((char**) vector_top(vector)));
  vector_pop(vector);
  printf("top: %s\n", *((char**) vector_top(vector)));
  
  char** str = &test[1];
  int idx = vector_find(vector, str);
  printf("idx: %d\n", idx);
  vector_free(vector);
  return 0;
}

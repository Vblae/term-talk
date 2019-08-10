#include <stdio.h> 

#include "tserv/default.h"
#include "config/config.h"
#include "util/vector.h"

int32_t main(int argc, char** argv) {

  config_s* conf = load_config(argv[1]);
  printf("p: %p\n", conf);

  printf("\nvector test start ------------------\n");
  vector_s* vector = vector_of_string_create(0);
  char* test[] = {
    "str 1",
    "str 2",
    (char*) 0,
    "str 3"
  };
  for(int32_t i = 0; i < 4; i++) {
    vector_push(vector, &test[i]);
  }
  for(int32_t i = 0; i < vector->len; i++) {
    char** val = (char**) vector_get(vector, i);
    printf("og:%p->%s new:%p->%s\n", test[i], test[i], *val, *val);
  }

  printf("back: %s\n", *((char**) vector_back(vector)));
  vector_pop(vector);
  printf("back: %s\n", *((char**) vector_back(vector)));
  
  char** str = &test[1];
  int32_t idx = vector_find(vector, str, vector_string_comparator);
  printf("idx: %d\n", idx);
  vector_free(vector);
  printf("vector test end ------------------\n");
  return 0;
}

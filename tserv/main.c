#include <stdio.h> 

#include "tserv/default.h"
#include "config/config.h"
#include "util/vector.h"

void __test(char* config_file_path) {
  config_s* conf = load_config(config_file_path);
  printf("p: %p\n", conf);

  config_var_s* _5 = config_get(conf, "_5");
  printf("get '_5': %p %s\n", _5, _5->name);

  config_var_s* none = config_get(conf, "none");
  printf("get 'none': %p\n", none);

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

  printf("back: %s\n", *(vector_back_of(char*, vector)));
  vector_pop(vector);
  printf("back: %s\n", *(vector_back_of(char*, vector)));
  
  char** str = &test[1];
  int32_t idx = vector_find(vector, str, vector_string_comparator);
  printf("idx: %d\n", idx);
  vector_free(vector);
  printf("vector test end ------------------\n");
}

int32_t main(int argc, char** argv) {
  __test(argv[1]);
  return 0;
}


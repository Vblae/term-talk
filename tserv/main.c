#include <stdio.h> 

#include "tserv/default.h"
#include "config/config.h"
#include "util/vector.h"
#include "util/treemap.h"

static void __test(char* config_file_path) {
  config_s* conf = load_config(config_file_path);
  printf("p: %p\n", conf);

  config_var_s* _5 = config_get(conf, "_5");
  printf("get '_5': %p %s\n", _5, _5->name);

  config_var_s* none = config_get(conf, "none");
  printf("get 'none': %p\n", none);

  tree_map_s temp;

  printf("temp test %lu\n", sizeof(temp));
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

int32_t int_comparator(void* ptr0, void* ptr1) {
  return *((int32_t*) ptr0) - *((int32_t*) ptr1);
}

void int_key_printer(void* key) {
  printf("%s", *((char**) key));
}

void float_val_printer(void* val) {
  printf("%i", *((int*) val));
}


static void __test_tree_map() {
  printf("calling tree_map_create() of int32_t => float\n");
  tree_map_s* map = tree_map_create_of(int32_t, float, &int_comparator);
  printf("returned from tree_map_create() with %p\n", map);
  
  int32_t keys[] = {12, 567, 43, 114, 8, 6, 3, 4};
  float vals[] = {1.2, 3.4, 0.4, 9.6, 2.3, 7.7, 8.9, 0.0};

  printf("calling tree_map_insert() with:\n");
  for(int32_t i = 0; i < 8; i++) {
    int32_t status = tree_map_insert(map, &keys[i], &vals[i]);
    printf("  k => v : %i => %f %s\n", keys[i], vals[i], status ? "OK" : "ERR");
    printf("   k* => v*: %p => %p\n", &keys[i], &vals[i]);
  }
  printf("finish inserting\n");
  printf("calling tree_map_get() with:\n");
  for(int32_t i = 0; i < 8; i++) {
    float* result = tree_map_get_of(float, map, &keys[i]);
    printf("query key: %d expect: %f got %p -> ", keys[i], vals[i], result);
    if(!result)
      printf("None\n");
    else
      printf("%f\n", *result);
  }
  tree_map_free(map);
}

static void __test_tree_map_string_keys() {
  printf("calling tree_map_create_of_string_key() of char* => int32_t\n");
  tree_map_s* map = tree_map_create_of_string_key(sizeof(int32_t));
  printf("returned from tree_map_create() with %p\n", map);
  
  char* keys[] = {
    "this key",
    "that key",
    "they key",
    "this key"
  };
  printf("WHAT WHAT %p %p\n", &keys[0], keys[0]);
  printf("WHAT WHAT %p %p\n", &keys[1], keys[1]);
  int32_t vals[] = {1, 2, 3, 10};

  printf("calling tree_map_insert() with:\n");
  for(int32_t i = 0; i < 4; i++) {
    printf("  k => v : %s => %i ", keys[i], vals[i]);
    int32_t status = tree_map_insert(map, &keys[i], &vals[i]);
    printf("%s\n", status ? "OK" : "ERR");
    printf("   k* => v*: %p => %p\n", &keys[i], &vals[i]);
  }
  printf("finish inserting\n");
  tree_map_print(map, int_key_printer, float_val_printer);
  printf("calling tree_map_get() with:\n");
  for(int32_t i = 0; i < 4; i++) {
    void* result = tree_map_get(map, &keys[i]);
    printf("query key: %s expect: %i got %p -> ", keys[i], vals[i], result);
    if(!result)
      printf("None\n");
    else
      printf("%i\n", *((int*) result));
  }
  tree_map_free(map);
}

int32_t main(int argc, char** argv) {
  __test(argv[1]);
  __test_tree_map();
  __test_tree_map_string_keys();
  return 0;
}


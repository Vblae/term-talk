#ifndef __CONFIG__
#define __CONFIG__

#include "parse.h"

#define MAX_VAR_LEN 255

struct config_var {
  struct config_var* next;
  char name[MAX_VAR_LEN + 1];

  data_type_t type;
  union {
    char byte_val;
    short short_val;
    int int_val;
    long long_val;
    float float_val;
    double double_val;
    char* string_val;
  };
};

typedef struct config_var config_var_s;

struct config {
  unsigned int size;
  config_var_s* vars;
  config_var_s* tail;
};

typedef struct config config_s;

config_s* create_config();

void free_config(config_s* conf);

config_s* load_config(char* config_file_path);

#endif


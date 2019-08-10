#ifndef __CONFIG__
#define __CONFIG__

#include <stdint.h>
#include <stddef.h>

#include "parse.h"

#define MAX_VAR_LEN 255

struct config_var {
  struct config_var* next;
  char name[MAX_VAR_LEN + 1];

  data_type_t type;
  union {
    int8_t int8_val;
    int16_t int16_val;
    int32_t int32_val;
    int64_t int64_val;
    float float_val;
    double double_val;
    char* string_val;
    
  };
};

typedef struct config_var config_var_s;

struct config {
  size_t size;
  config_var_s* vars;
  config_var_s* tail;
};

typedef struct config config_s;

config_s* create_config();

void free_config(config_s* conf);

config_s* load_config(char* config_file_path);

#endif


#ifndef __CONFIG__
#define __CONFIG__

#include <stdint.h>
#include <stddef.h>

#include "parse.h"

#define MAX_VAR_LEN 255

struct config_var {
  char name[MAX_VAR_LEN + 1];

  data_type_t type;
  union {
    int8_t int8_val;
    int16_t int16_val;
    int32_t int32_val;
    int64_t int64_val;
    int8_t uint8_val;
    int16_t uint16_val;
    int32_t uint32_val;
    int64_t uint64_val;
    float float_val;
    double double_val;
    char* string_val;
  };

  struct config_var* left;
  struct config_var* right;
};

typedef struct config_var config_var_s;

struct config {
  config_var_s* root;
};

typedef struct config config_s;

config_s* config_create();

void config_freg(config_s* conf);

config_var_s* config_get(config_s* conf, char* name);

config_s* load_config(char* config_file_path);

#endif


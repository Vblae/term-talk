#ifndef __CONFIG__
#define __CONFIG__

#define MAX_VAL_LEN 255
#define MAX_VAR_LEN 255

struct config_var {
  struct config_var* next;
  char name[MAX_VAR_LEN + 1];

  int type;
  union {
    char byte_val;
    short short_val;
    int int_val;
    long long_val;
    float float_val;
    double double_val;
    char string_val[MAX_VAL_LEN + 1];
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


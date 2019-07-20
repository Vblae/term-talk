#ifndef __TSERV_CONFIG__
#define __TSERV_CONFIG__

#define BYTE_VAR 0x10
#define INT_VAR 0x20
#define SHORT_VAR 0x30
#define LONG_VAR 0x40
#define FLOAT_VAR 0x50
#define DOUBLE_VAR 0x60
#define CHAR_VAR 0x70
#define STRING_VAR 0x80

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
    char char_val;
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


#ifndef __PARSE_H__
#define __PARSE_H__

#include "util/vector.h"

#define NONE_TYPE 0x00
#define BYTE_TYPE 0x10
#define SHORT_TYPE 0x20
#define INT_TYPE 0x30
#define LONG_TYPE 0x40
#define FLOAT_TYPE 0x50
#define DOUBLE_TYPE 0x60
#define STRING_TYPE 0x80

#define NONE_TYPE_SPECIFIER "None"
#define BYTE_TYPE_SPECIFIER "i8"
#define SHORT_TYPE_SPECIFIER "i16"
#define INT_TYPE_SPECIFIER "i32"
#define LONG_TYPE_SPECIFIER "i64"
#define FLOAT_TYPE_SPECIFIER "f32"
#define DOUBLE_TYPE_SPECIFIER "f64"
#define STRING_TYPE_SPECIFIER "string"

typedef int data_type_t;

struct parse_result {
  char* var_name;
  void* var_data;

  data_type_t var_type;
  int success;
};

typedef struct parse_result parse_result_s;

char* data_type_to_string(data_type_t type);

vector_s* parse_lines(int fd);

#endif


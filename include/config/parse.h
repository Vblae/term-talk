#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdint.h>

#include "util/vector.h"

#define NONE_TYPE 0x00
#define INT8_TYPE 0x10
#define INT16_TYPE 0x20
#define INT32_TYPE 0x30
#define INT64_TYPE 0x40
#define FLOAT_TYPE 0x50
#define DOUBLE_TYPE 0x60
#define STRING_TYPE 0x80

#define NONE_TYPE_SPECIFIER "None"
#define INT8_TYPE_SPECIFIER "i8"
#define INT16_TYPE_SPECIFIER "i16"
#define INT32_TYPE_SPECIFIER "i32"
#define INT64_TYPE_SPECIFIER "i64"
#define FLOAT_TYPE_SPECIFIER "f32"
#define DOUBLE_TYPE_SPECIFIER "f64"
#define STRING_TYPE_SPECIFIER "string"

typedef int32_t data_type_t;

struct parse_result {
  char* var_name;
  void* var_data;

  data_type_t var_type;
  int32_t success;
};

typedef struct parse_result parse_result_s;

char* data_type_to_string(data_type_t type);

vector_s* parse_lines(int32_t fd);

#endif


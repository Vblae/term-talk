#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdint.h>

#include "util/vector.h"

typedef enum {
  TYPE_NONE = 0x00,
  TYPE_INT8 = 0x10,
  TYPE_INT16 = 0x20,
  TYPE_INT32 = 0x30,
  TYPE_INT64 = 0x40,
  TYPE_UINT8 = 0x50,
  TYPE_UINT16 = 0x60,
  TYPE_UINT32 = 0x70,
  TYPE_UINT64 = 0x80,
  TYPE_FLOAT = 0x90,
  TYPE_DOUBLE = 0xA0,
  TYPE_STRING = 0xB0,
} data_type_e;

struct parse_result {
  char* var_name;
  void* var_data;

  data_type_e var_type;
  int32_t success;
};

typedef struct parse_result parse_result_s;

const char* data_type_to_string(data_type_e type);

vector_s* parse_lines(int32_t fd);

#endif


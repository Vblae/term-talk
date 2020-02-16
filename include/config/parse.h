#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdint.h>

#include "util/vector.h"

#define TYPE_NONE 0x00
#define TYPE_INT8 0x10
#define TYPE_INT16 0x20
#define TYPE_INT32 0x30
#define TYPE_INT64 0x40
#define TYPE_UINT8 0x50
#define TYPE_UINT16 0x60
#define TYPE_UINT32 0x70
#define TYPE_UINT64 0x80
#define TYPE_FLOAT 0x90
#define TYPE_DOUBLE 0xA0
#define TYPE_STRING 0xB0

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


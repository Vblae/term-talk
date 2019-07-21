#ifndef __PARSE_H__
#define __PARSE_H__

#define NONE_VAR 0x00
#define BYTE_VAR 0x10
#define INT_VAR 0x20
#define SHORT_VAR 0x30
#define LONG_VAR 0x40
#define FLOAT_VAR 0x50
#define DOUBLE_VAR 0x60
#define CHAR_VAR 0x70
#define STRING_VAR 0x80

typedef int data_type_t;

struct parse_result {
  char* var_name;
  void* var_data;

  data_type_t var_type;
  int success;
};

typedef struct parse_result parse_result_s;

void parse_line(char* line, size_t line_len, parse_result_s* parse_res);

#endif


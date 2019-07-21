#include <string.h>
#include <stddef.h>
#include <stdio.h>

#include "config/config.h"
#include "config/parse.h"


static inline void __return_none_var(parse_result_s* parse_res) {
    parse_res->success = 1;
    parse_res->var_name = (char*) 0;
    parse_res->var_data = (void*) 0;
    parse_res->var_type = NONE_VAR;
}

void parse_line(char* line, size_t line_len, parse_result_s* parse_res) {
  if(!line || !line_len)
    return __return_none_var(parse_res);

  printf("line[%lu]: %s\n", line_len, line);
}

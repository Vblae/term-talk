#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "config/config.h"
#include "config/parse.h"
#include "util/vector.h"
#include "util/log.h"

static inline int __is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline int __is_digit(char c) {
  return c >= '0' && c <= '9';
}

static inline int __is_underscore(char c) {
  return c == '_';
}

static inline int __is_colon(char c) {
  return c == ':';
}

static inline int __is_point(char c) {
  return c == '.';
}

static inline int __is_negative_sign(char c) {
  return c == '-';
}

static inline int __is_single_quote(char c) {
  return c == '\'';
}

static inline int __is_white_space(char c) {
  return c == ' ' || c == '\r' || c == '\t';
}

static data_type_t __is_type_specifier(char* type) {
  if(strcmp(type, BYTE_TYPE_SPECIFIER) == 0)
    return BYTE_TYPE;

  if(strcmp(type, SHORT_TYPE_SPECIFIER) == 0)
    return SHORT_TYPE;

  if(strcmp(type, INT_TYPE_SPECIFIER) == 0)
    return INT_TYPE;

  if(strcmp(type, LONG_TYPE_SPECIFIER) == 0)
    return LONG_TYPE;

  if(strcmp(type, FLOAT_TYPE_SPECIFIER) == 0)
    return FLOAT_TYPE;

  if(strcmp(type, DOUBLE_TYPE_SPECIFIER) == 0)
    return DOUBLE_TYPE;

  if(strcmp(type, STRING_TYPE_SPECIFIER) == 0)
    return STRING_TYPE;

  return NONE_TYPE;
}

static int __is_name(char* id) {
  if(!id|| !*id)
    return 0;

  char* it = id;
  while(*it) {
    if(it == id && !__is_alpha(*it) && !__is_underscore(*it))
      return 0;
    else if(it != id && !__is_alpha(*it) && !__is_digit(*it) && !__is_underscore(*it))
      return 0;

    it++;
  }

  return !__is_type_specifier(id);
}

static int __is_integer(char* integer) {
  if(!integer || !*integer)
    return 0;

  char* it = integer;
  while(*it) {
    if(it == integer && !__is_negative_sign(*it) && !__is_digit(*it))
      return 0;
    else if(it != integer && !__is_digit(*it))
      return 0;

    it++;
  }

  return 1;
}

static int __is_real_number(char* real) {
  if(!real || !*real)
    return 0;

  char* it = real;
  int saw_decimal_point = 0;
  while(*it) {
    if(it == real && !__is_digit(*it))
      return 0;
    else if(it != real && __is_point(*it) && saw_decimal_point)
      return 0;
    else if(it != real && __is_point(*it) && !saw_decimal_point)
      saw_decimal_point = 1;
    else if(it != real && !__is_digit(*it))
      return 0;

    it++;
  }

  return 1;
}

static int __is_string_literal(char* string_literal) {
  if(!string_literal)
    return 0;
  
  size_t literal_len;
  if((literal_len = strlen(string_literal)) < 2)
    return 0;

  return string_literal[0] == '\'' && string_literal[literal_len - 1] == '\'';
}

static char* __data_type_to_string(data_type_t type) {
  switch(type) {
    case BYTE_TYPE:
      return BYTE_TYPE_SPECIFIER;
    case SHORT_TYPE:
      return SHORT_TYPE_SPECIFIER;
    case INT_TYPE:
      return INT_TYPE_SPECIFIER;
    case LONG_TYPE:
      return LONG_TYPE_SPECIFIER;
    case FLOAT_TYPE:
      return FLOAT_TYPE_SPECIFIER;
    case DOUBLE_TYPE:
      return DOUBLE_TYPE_SPECIFIER;
    case STRING_TYPE:
      return STRING_TYPE_SPECIFIER;
  }

  return NONE_TYPE_SPECIFIER;
}

static inline data_type_t __soft_type_of(char* value) {
  if(__is_integer(value))
    return INT_TYPE;

  if(__is_real_number(value))
    return FLOAT_TYPE;

  if(__is_string_literal(value))
    return STRING_TYPE;

  return NONE_TYPE;
}
static int __tokenize_line(
  char* line,
  size_t line_len,
  size_t line_num,
  vector_s* vector
) {
  if(!line || !line_len)
    return 0;

  if(!vector) {
    m_log_error(
      "error: parser: ivalid argument given to __tokenize_line vector cannot be null\n"
    );
    return 0;
  }
  
  char* it = line;
  char* end = line + line_len;
  *end = 0;

  char* token_copy = 0;
  char* token_start= 0;
  size_t token_len = 0;
  
  const int INVALID_STATE = -3;
  const int PUSH_STATE = -2;
  const int INIT_STATE = -1;
  const int START_STATE = 0;
  const int FORM_WORD = 1;
  const int FORM_NUMBER = 2;
  const int FORM_SYMBOL = 3;
  const int FORM_STRING = 4;
  const int SKIP_WHITE_SPACE = 5;

  int state = START_STATE;
  int next_state = START_STATE;
  while(it < end) {
    switch(state) {
      case PUSH_STATE:
        token_copy = (char*) malloc(token_len + 1);
        memcpy(token_copy, token_start, token_len);
        token_copy[token_len] = 0;

        vector_push(vector, &token_copy);
        next_state = INIT_STATE;
        break;
      case INIT_STATE:
        token_copy = 0;
        token_start = 0;
        token_len = 0;
        next_state = START_STATE;
        break;
      case START_STATE:
        if(__is_white_space(*it))
          next_state = SKIP_WHITE_SPACE;
        else if(__is_underscore(*it) || __is_alpha(*it)) 
          next_state = FORM_WORD;
        else if(__is_negative_sign(*it) || __is_digit(*it) || __is_point(*it))
          next_state = FORM_NUMBER;
        else if(__is_colon(*it))
          next_state = FORM_SYMBOL;
        else if(__is_single_quote(*it))
          next_state = FORM_STRING;
        else
          next_state = INVALID_STATE;

        switch(next_state) {
          case FORM_WORD:
          case FORM_NUMBER:
          case FORM_SYMBOL:
          case FORM_STRING:
            token_start = it;
            token_len++;
            it++;
            break;
        }
        break;
      case FORM_WORD:
        if(!__is_underscore(*it) && !__is_alpha(*it) && !__is_digit(*it)) {
          next_state = PUSH_STATE;
        } else {
          token_len++;
          it++;
        }
        break;
      case FORM_NUMBER:
        if(!__is_digit(*it) && !__is_negative_sign(*it) && !__is_point(*it)) {
          next_state = PUSH_STATE;
        } else {
          token_len++;
          it++;
        }
        break;
      case FORM_SYMBOL:
        if(!__is_colon(*it)) {
          next_state = PUSH_STATE;
        } else {
          token_len++;
          it++;
        }
        break;
      case FORM_STRING:
        if(__is_single_quote(*it))
          next_state = PUSH_STATE;

        token_len++;
        it++;
        break;
      case SKIP_WHITE_SPACE:
        if(!__is_white_space(*it))
          next_state = START_STATE;
        else
          it++;
        break;
      case INVALID_STATE:
      default:
        m_log_error(
          "error: parser: invalid character '%c' in line %lu\n=>%s\n",
          *it,
          line_num,
          line
        );
        return 0;
    }  

    if(it == end && token_start != 0 && token_len > 0) {
      token_copy = (char*) malloc(token_len + 1);
      memcpy(token_copy, token_start, token_len);
      token_copy[token_len] = 0;

      vector_push(vector, &token_copy);
    }

    state = next_state;
  }

  return 1;
}

static inline void __make_none_var(parse_result_s* parse_res) {
    parse_res->success = 1;
    parse_res->var_name = (char*) 0;
    parse_res->var_data = (void*) 0;
    parse_res->var_type = NONE_TYPE;
}

static void __match_var_decleration(
  char* line,
  size_t line_len,
  size_t line_num,
  parse_result_s* parse_res,
  vector_s* vector
) {
  int success = __tokenize_line(line, line_len, line_num, vector);
  if(!success) {
    __make_none_var(parse_res);
    return;
  }
  
  char** type_specifier = (char**) vector_get(vector, 0);
  char** var_name = (char**) vector_get(vector, 1);
  char** colon = (char**) vector_get(vector, 2);
  char** value_as_string = (char**) vector_get(vector, 3);
  
  int var_type;
  if(!(var_type = __is_type_specifier(*type_specifier))) {
    m_log_error(
      "error: parser: invalid type '%s' in line %lu\n==>%s\n",
      *type_specifier,
      line_num,
      line
    );
    __make_none_var(parse_res);
    return;
  }

  if(!__is_name(*var_name)) {
    m_log_error(
      "error: parser: invalid variable name '%s' in line %lu\n==> %s\n",
      *var_name,
      line_num,
      line
    );

    __make_none_var(parse_res);
    return;
  }

  if(!__is_colon(**colon)) {
    m_log_error(
      "error: parser: expected symbol ':' but got '%s' instead in line %lu\n==> %s\n",
      *colon,
      line_num,
      line
    );

    __make_none_var(parse_res);
    return; 
  }

  if(vector->len > 4) {
    m_log_error(
      "error: parser: unexpected token '%s' in line %lu\n==> %s\n",
      *((char**) vector_get(vector, 4)),
      line_num,
      line
    );
    
    __make_none_var(parse_res);
    return;
  }

  char* type_received = __data_type_to_string(__soft_type_of(*value_as_string));
  int type_error = 0;
  switch(var_type) {
    case BYTE_TYPE:
    case SHORT_TYPE:
    case INT_TYPE:
    case LONG_TYPE:
      if(!__is_integer(*value_as_string))
        type_error = 1; 

      break;
    case FLOAT_TYPE:
    case DOUBLE_TYPE:
      if(!__is_real_number(*value_as_string))
        type_error = 1;
      break;
    case STRING_TYPE:
      if(!__is_string_literal(*value_as_string))
        type_error = 1;
      break;
  }
  
  if(type_error) {
    m_log_error(
      "error: parser: expected value of type '%s' but got '%s' of type '%s'"
        " in line %lu\n==> %s\n",
      *type_specifier,
      *value_as_string,
      type_received,
      line_num,
      line
    );

    __make_none_var(parse_res);
    return;
  }

  parse_res->var_name = strdup(*var_name);
  parse_res->var_data = strdup(*value_as_string);
  parse_res->var_type = var_type;
  parse_res->success = 1;
}

parse_result_s* create_parse_result() {
  void* new_parse_result = malloc(sizeof(parse_result_s));
  memset(new_parse_result, 0, sizeof(parse_result_s));
  return (parse_result_s*) new_parse_result;
}

void free_parse_result(parse_result_s* parse_res) {
  free(parse_res);
}

void parse_line(
  char* line,
  size_t line_len,
  size_t line_num,
  parse_result_s* parse_res,
  vector_s* vector
) {
  if(!line || !line_len)
    __make_none_var(parse_res);
  else
    __match_var_decleration(line, line_len, line_num, parse_res, vector);

  for(int i = 0; i < vector->len; i++) {
    char* ptr = *((char**) vector_get(vector, i));
    free(ptr);
  }

  vector_clear(vector);
}


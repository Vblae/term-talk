#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "config/config.h"
#include "config/parse.h"
#include "util/vector.h"
#include "util/log.h"
#include "util/stringutil.h"

#define BUFF_LEN 1024

#define TYPE_SPECIFIER_NONE "None"
#define TYPE_SPECIFIER_INT8 "i8"
#define TYPE_SPECIFIER_INT16 "i16"
#define TYPE_SPECIFIER_INT32 "i32"
#define TYPE_SPECIFIER_INT64 "i64"
#define TYPE_SPECIFIER_UINT8 "u8"
#define TYPE_SPECIFIER_UINT16 "u16"
#define TYPE_SPECIFIER_UINT32 "u32"
#define TYPE_SPECIFIER_UINT64 "u64"
#define TYPE_SPECIFIER_FLOAT "f32"
#define TYPE_SPECIFIER_DOUBLE "f64"
#define TYPE_SPECIFIER_STRING "string"

static inline int32_t __is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline int32_t __is_digit(char c) {
  return c >= '0' && c <= '9';
}

static inline int32_t __is_underscore(char c) {
  return c == '_';
}

static inline int32_t __is_colon(char c) {
  return c == ':';
}

static inline int32_t __is_point(char c) {
  return c == '.';
}

static inline int32_t __is_negative_sign(char c) {
  return c == '-';
}

static inline int32_t __is_single_quote(char c) {
  return c == '\'';
}

static inline int32_t __is_white_space(char c) {
  return c == ' ' || c == '\r' || c == '\t';
}

static data_type_e __is_type_specifier(char* type) {
  if(strcmp(type, TYPE_SPECIFIER_INT8) == 0)
    return TYPE_INT8;

  if(strcmp(type, TYPE_SPECIFIER_INT16) == 0)
    return TYPE_INT16;

  if(strcmp(type, TYPE_SPECIFIER_INT32) == 0)
    return TYPE_INT32;

  if(strcmp(type, TYPE_SPECIFIER_INT64) == 0)
    return TYPE_INT64;

  if(strcmp(type, TYPE_SPECIFIER_UINT8) == 0)
    return TYPE_UINT8;

  if(strcmp(type, TYPE_SPECIFIER_UINT16) == 0)
    return TYPE_UINT16;

  if(strcmp(type, TYPE_SPECIFIER_UINT32) == 0)
    return TYPE_UINT32;

  if(strcmp(type, TYPE_SPECIFIER_UINT64) == 0)
    return TYPE_UINT64;
  
  if(strcmp(type, TYPE_SPECIFIER_FLOAT) == 0)
    return TYPE_FLOAT;

  if(strcmp(type, TYPE_SPECIFIER_DOUBLE) == 0)
    return TYPE_DOUBLE;

  if(strcmp(type, TYPE_SPECIFIER_STRING) == 0)
    return TYPE_STRING;

  return TYPE_NONE;
}

static int32_t __is_name(char* id) {
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

static int32_t __is_integer(char* integer) {
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

static int32_t __is_real_number(char* real) {
  if(!real || !*real)
    return 0;

  char* it = real;
  int32_t saw_decimal_point = 0;
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

static int32_t __is_string_literal(char* string_literal) {
  if(!string_literal)
    return 0;
  
  size_t literal_len;
  if((literal_len = strlen(string_literal)) < 2)
    return 0;

  return string_literal[0] == '\'' && string_literal[literal_len - 1] == '\'';
}

const char* data_type_to_string(data_type_e type) {
  switch(type) {
    case TYPE_INT8:
      return TYPE_SPECIFIER_INT8;
    case TYPE_INT16:
      return TYPE_SPECIFIER_INT16;
    case TYPE_INT32:
      return TYPE_SPECIFIER_INT32;
    case TYPE_INT64:
      return TYPE_SPECIFIER_INT64;
    case TYPE_UINT8:
      return TYPE_SPECIFIER_UINT8;
    case TYPE_UINT16:
      return TYPE_SPECIFIER_UINT16;
    case TYPE_UINT32:
      return TYPE_SPECIFIER_UINT32;
    case TYPE_UINT64:
      return TYPE_SPECIFIER_UINT64;
    case TYPE_FLOAT:
      return TYPE_SPECIFIER_FLOAT;
    case TYPE_DOUBLE:
      return TYPE_SPECIFIER_DOUBLE;
    case TYPE_STRING:
      return TYPE_SPECIFIER_STRING;
    default:
      return TYPE_SPECIFIER_NONE;
  }
}

static inline data_type_e __soft_type_of(char* value) {
  if(__is_integer(value)) {
    return TYPE_INT32;
  }

  if(__is_real_number(value))
    return TYPE_FLOAT;

  if(__is_string_literal(value))
    return TYPE_STRING;

  return TYPE_NONE;
}
static int32_t __tokenize_line(
  char* line,
  size_t line_len,
  size_t line_num,
  vector_s* vector
) {
  if(!line || !line_len)
    return 0;

  if(!vector) {
    LOGE(
      "parser: error: ivalid argument given to __tokenize_line vector cannot be null\n"
    );
    return 0;
  }
  
  char* it = line;
  char* end = line + line_len;
  *end = 0;

  char* token_copy = 0;
  char* token_start= 0;
  size_t token_len = 0;
  
  const int32_t INVALID_STATE = -3;
  const int32_t PUSH_STATE = -2;
  const int32_t INIT_STATE = -1;
  const int32_t START_STATE = 0;
  const int32_t FORM_WORD = 1;
  const int32_t FORM_NUMBER = 2;
  const int32_t FORM_SYMBOL = 3;
  const int32_t FORM_STRING = 4;
  const int32_t SKIP_WHITE_SPACE = 5;

  int32_t state = START_STATE;
  int32_t next_state = START_STATE;
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
        else if(it == end - 1) {
          LOGE(
            "parser: error: unclosed string literal [%s] in line %lu\n",
            token_start,
            line_num
          );

          return 0;
        }

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
        LOGE(
          "parser: error: invalid character '%c' in line %lu\n=>%s\n",
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
    parse_res->var_type = TYPE_NONE;
}

static void __match_var_decleration(
  char* line,
  size_t line_len,
  size_t line_num,
  parse_result_s* parse_res,
  vector_s* vector
) {
  int32_t success = __tokenize_line(line, line_len, line_num, vector);
  if(!success) {
    __make_none_var(parse_res);
    return;
  }
  
  char** type_specifier = vector_get_of(char*, vector, 0);
  char** var_name = vector_get_of(char*, vector, 1);
  char** colon = vector_get_of(char*, vector, 2);
  char** value_as_string = vector_get_of(char*, vector, 3);
  
  data_type_e var_type;
  if(!(var_type = __is_type_specifier(*type_specifier))) {
    LOGE(
      "parser: error: invalid type '%s' in line %lu\n==>%s\n",
      *type_specifier,
      line_num,
      line
    );

    __make_none_var(parse_res);
    return;
  }

  if(!__is_name(*var_name)) {
    LOGE(
      "parser: error: invalid variable name '%s' in line %lu\n==> %s\n",
      *var_name,
      line_num,
      line
    );

    __make_none_var(parse_res);
    return;
  }

  if(!__is_colon(**colon)) {
    LOGE(
      "parser: error: expected symbol ':' but got '%s' instead in line %lu\n==> %s\n",
      *colon,
      line_num,
      line
    );

    __make_none_var(parse_res);
    return; 
  }

  if(vector->len > 4) {
    LOGE(
      "parser: error: unexpected token '%s' in line %lu\n==> %s\n",
      *((char**) vector_get(vector, 4)),
      line_num,
      line
    );
    
    __make_none_var(parse_res);
    return;
  }

  const char* type_received = data_type_to_string(__soft_type_of(*value_as_string));
  int32_t type_error = 0;
  switch(var_type) {
    case TYPE_INT8:
    case TYPE_INT16:
    case TYPE_INT32:
    case TYPE_INT64:
      if(!__is_integer(*value_as_string))
        type_error = 1; 

      break;
    case TYPE_UINT8:
    case TYPE_UINT16:
    case TYPE_UINT32:
    case TYPE_UINT64:
      if(!__is_integer(*value_as_string) || __is_negative_sign(**value_as_string))
        type_error = 1;

      break;
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
      if(!__is_real_number(*value_as_string))
        type_error = 1;
      break;
    case TYPE_STRING:
      if(!__is_string_literal(*value_as_string))
        type_error = 1;
      break;
    case TYPE_NONE:
      // getting here should not happen, if it does then
      // you should fix it
      LOGE("parser: error: invalid type 'NONE' received\n");
      __make_none_var(parse_res);
      return;
  }
  
  if(type_error) {
    LOGE(
      "parser: error: expected value of type '%s' but got '%s' of type '%s'"
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

  char* var_data_as_string = 0;
  if(var_type != TYPE_STRING) {
    var_data_as_string = strdup(*value_as_string);
  } else {
    size_t len = strlen(*value_as_string);
    // strip closing string literal '
    (*value_as_string)[len - 1] = 0;
    // strip opening string literal '
    var_data_as_string = strdup((*value_as_string) + 1);
  }

  parse_res->var_name = strdup(*var_name);
  parse_res->var_data = var_data_as_string;
  parse_res->var_type = var_type;
  parse_res->success = 1;
}

void __parse_line(
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

  for(int32_t i = 0; i < vector->len; i++) {
    char** ptr = vector_get_of(char*, vector, i);
    free(*ptr);
  }

  vector_clear(vector);
}

static void __shift_buff_left(
  char* buff,
  int32_t start_index,
  int32_t shift_amnt,
  size_t buff_len
) {
  if(shift_amnt >= buff_len) {
    memset(buff, 0, buff_len);
    return;
  }

  for(int32_t i = 0; i < shift_amnt; i++) {
    buff[i] = buff[start_index + i];
  }

  memset(&buff[shift_amnt], 0, buff_len - shift_amnt);
}

static void __parse_line_result_deallocator(void* parse_res) {
  if(!parse_res)
    return;

  parse_result_s* parse_res_ptr = (parse_result_s*) parse_res;
  free(parse_res_ptr->var_name);
  free(parse_res_ptr->var_data);
}

vector_s* parse_lines(int32_t fd) {
  size_t buff_len = BUFF_LEN;
  char buff[buff_len];
  memset(&buff, 0, buff_len);

  vector_s* vector_for_parse = vector_of_string_create(5);
  vector_s* parse_results = vector_create_with_allocators(
    0,
    sizeof(parse_result_s),
    0,
    &__parse_line_result_deallocator
  );

  int32_t line_num = 1;
  size_t buff_offset = 0;
  while(1) {
    size_t bytes_read = 0;
    size_t bytes_overflowed = 0;
    size_t expected_bytes_read = buff_len - buff_offset;
    while((bytes_read = read(fd, &buff[buff_offset], expected_bytes_read)) > 0) {
      if(bytes_read < expected_bytes_read)
        buff_len = bytes_read + bytes_overflowed;
      
      buff_offset = 0;
      int32_t nl_index = 0;
      int32_t saw_nl = 0;
      while(
        (nl_index = index_of('\n', &buff[buff_offset], buff_len - buff_offset)) != -1
      ) {
        saw_nl = 1;
        if(nl_index == 0) {
          buff_offset++;
          line_num++;
          continue;
        }

        char line[nl_index + 1];
        memcpy(line, &buff[buff_offset], nl_index);
        line[nl_index] = 0;
        
        parse_result_s parse_res;
        __parse_line(line, nl_index, line_num, &parse_res, vector_for_parse);
        
        if(!parse_res.success || parse_res.var_type == TYPE_NONE) {
          vector_free(parse_results);
          vector_free(vector_for_parse);
          return NULL;
        }

        vector_push(parse_results, &parse_res);
        buff_offset += nl_index + 1;
        line_num++;
      }

      char check = 0;
      size_t check_read = read(fd, &check, 1);

      if(buff_offset == buff_len && check_read == 0) {
        vector_free(vector_for_parse);
        return parse_results;
      }

      if(!saw_nl && check_read == 0) {
        char line[bytes_overflowed + bytes_read + 1];
        memcpy(line, &buff[buff_offset], bytes_overflowed + bytes_read);
        line[bytes_overflowed + bytes_read] = 0;

        parse_result_s parse_res;
        __parse_line(
          line,
          bytes_overflowed + bytes_read,
          line_num,
          &parse_res,
          vector_for_parse
        );

        if(!parse_res.success || parse_res.var_type == TYPE_NONE) {
          vector_free(parse_results);
          vector_free(vector_for_parse);
          return NULL;
        }

        vector_push(parse_results, &parse_res);
        vector_free(vector_for_parse);
        return parse_results;
      }
      
      if(!saw_nl && check_read == 1 && check != '\n') {
        LOGE("parser: error: line %d: line is too long\n", line_num);
        vector_free(parse_results);
        vector_free(vector_for_parse);
        return NULL;
      }
      
      if(!saw_nl && check_read == 1 && check == '\n') {
        char line[bytes_overflowed + bytes_read + 1];
        memcpy(line, &buff[buff_offset], bytes_overflowed + bytes_read);
        line[bytes_overflowed + bytes_read] = 0;

        parse_result_s parse_res;
        __parse_line(
          line,
          bytes_overflowed + bytes_read,
          line_num,
          &parse_res,
          vector_for_parse
        );

        if(!parse_res.success || parse_res.var_type == TYPE_NONE) {
          vector_free(parse_results);
          vector_free(vector_for_parse);
          return NULL;
        }

        vector_push(parse_results, &parse_res);
        buff_offset += bytes_overflowed + bytes_read; 
        lseek(fd, -1, SEEK_CUR);
      } else if(saw_nl) {
        if(check_read == 0) {
          char line[buff_len - buff_offset + 1];
          memcpy(line, &buff[buff_offset], buff_len - buff_offset);
          line[buff_len - buff_offset] = 0;

          parse_result_s parse_res;
          __parse_line(
            line,
            buff_len - buff_offset,
            line_num,
            &parse_res,
            vector_for_parse
          );

          if(!parse_res.success || parse_res.var_type == TYPE_NONE) {
            vector_free(parse_results);
            vector_free(vector_for_parse);
            return NULL;
          }

          vector_push(parse_results, &parse_res);
          vector_free(vector_for_parse);
          return parse_results;
        }
        
        lseek(fd, -1, SEEK_CUR);
      }

      bytes_overflowed = buff_len - buff_offset;
      __shift_buff_left(buff, buff_offset, bytes_overflowed, buff_len);

      buff_offset = bytes_overflowed;
      expected_bytes_read = buff_len - buff_offset;
    }

    if(bytes_read <= 0)
      break;
  }

  vector_free(vector_for_parse);
  return parse_results;
}


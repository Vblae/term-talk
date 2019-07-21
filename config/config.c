#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "config/config.h"
#include "config/parse.h"

#define BUFF_LEN 100

static int __find_char(char* buff, char c, size_t buff_len);

static void __shift_buff_left(char* buff, int start_index, int shift_amnt, size_t buff_len);

config_s* create_config() {
  config_s* conf = (config_s*) malloc(sizeof(config_s));
  conf->vars = 0;
  conf->tail = 0;

  return conf;
}

void free_config(config_s* conf) {
  config_var_s* conf_var = conf->vars;
  while(conf_var) {
    config_var_s* next = conf_var->next;
    free(conf_var);

    conf_var = next;
  }

  free(conf);
}

config_var_s* __create_config_var(char* name, void* data, data_type_t type) {
  config_var_s* conf_var = (config_var_s*) malloc(sizeof(config_var_s));
  strncpy(&conf_var->name[0], name, MAX_VAR_LEN);

  switch(type) {
    case BYTE_VAR:
    case CHAR_VAR:
      conf_var->byte_val = *((char*) data);
     break;
    case SHORT_VAR:
      conf_var->short_val = *((short*) data);
      break;
    case INT_VAR:
      conf_var->int_val = *((int*) data);
      break;
    case LONG_VAR:
      conf_var->long_val = *((long*) data);
      break;
    case FLOAT_VAR:
      conf_var->float_val = *((float*) data);
      break;
    case DOUBLE_VAR:
      conf_var->double_val = *((double*) data);
      break;
    case STRING_VAR:
      strncpy(&conf_var->string_val[0], data, MAX_VAL_LEN);
      break;
    default:
      break;
  }

  return conf_var;
}

static void __add_config_var_head(config_s* conf, config_var_s* conf_var) {
  conf->vars = conf_var;
  conf->tail = conf_var;
}

static void __add_config_var_tail(config_s* conf, config_var_s* conf_var) {
  conf->tail->next = conf_var;
  conf->tail = conf_var;
}

void __add_config_var(config_s* conf, config_var_s* conf_var) {
  if(!conf->vars)
    __add_config_var_head(conf, conf_var);
  else
    __add_config_var_tail(conf, conf_var);
}

config_s* load_config(char* config_file_path) {
  if(!config_file_path) {
    printf("error: invalid config file path '%s'\n", config_file_path);
    return 0;
  }

  int fd = open(config_file_path, O_RDONLY);
  if(fd == -1) {
    printf("error: could not open file '%s'\n", config_file_path);
    return 0;
  }
  

  size_t buff_len = BUFF_LEN;
  char buff[buff_len];
  memset(&buff, 0, buff_len);

  int line_num = 1;
  size_t buff_offset = 0;
  config_s* conf = create_config();
  while(1) {
    size_t bytes_read = 0;
    size_t bytes_overflowed = 0;
    size_t expected_bytes_read = buff_len - buff_offset;
    while((bytes_read = read(fd, &buff[buff_offset], expected_bytes_read)) > 0) {
      if(bytes_read < expected_bytes_read)
        buff_len = bytes_read + bytes_overflowed;

      buff_offset = 0;
      int nl_index = 0;
      int saw_nl = 0;
      while((nl_index = __find_char(&buff[buff_offset], '\n', buff_len - buff_offset)) != -1) {
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
        parse_line(line, nl_index, &parse_res);
        
        //if(parse_res.success && parse_res.var_type != NONE_VAR) {
        //config_var_s* conf_var = __create_config_var(parse_res.var_name, parse_res.var_data, parse_res.var_type);
        //__add_config_var(conf, conf_var);
        //}

        buff_offset += nl_index + 1;
        line_num++;
      }

      char check = 0;
      size_t check_read = read(fd, &check, 1);
      if(!saw_nl && check_read == 0) {
        char line[bytes_overflowed + bytes_read + 1];
        memcpy(line, &buff[buff_offset], bytes_overflowed + bytes_read);
        line[bytes_overflowed + bytes_read] = 0;

        parse_result_s parse_res;
        parse_line(line, bytes_overflowed + bytes_read, &parse_res);
        
        //if(parse_res.success && parse_res.var_type != NONE_VAR) {
        //config_var_s* conf_var = __create_config_var(parse_res.var_name, parse_res.var_data, parse_res.var_type);
        //__add_config_var(conf, conf_var);
        //}
        
        return conf;
      }
      
      if(!saw_nl && check_read == 1 && check != '\n') {
        printf("error: line %d: line is too long\n", line_num);
        free(conf);
        return 0;
      }
      
      if(!saw_nl && check_read == 1 && check == '\n') {
        char line[bytes_overflowed + bytes_read + 1];
        memcpy(line, &buff[buff_offset], bytes_overflowed + bytes_read);
        line[bytes_overflowed + bytes_read] = 0;

        parse_result_s parse_res;
        parse_line(line, bytes_overflowed + bytes_read, &parse_res);
        
        //if(parse_res.success && parse_res.var_type != NONE_VAR) {
        //config_var_s* conf_var = __create_config_var(parse_res.var_name, parse_res.var_data, parse_res.var_type);
        //__add_config_var(conf, conf_var);
        //}
        
        buff_offset += bytes_overflowed + bytes_read; 
        lseek(fd, -1, SEEK_CUR);
      } else if(saw_nl) {
        if(check_read == 0) {
          char line[buff_len - buff_offset + 1];
          memcpy(line, &buff[buff_offset], buff_len - buff_offset);
          line[buff_len - buff_offset] = 0;

          parse_result_s parse_res;
          parse_line(line, buff_len - buff_offset, &parse_res);

          //if(parse_res.success && parse_res.var_type != NONE_VAR) {
          //config_var_s* conf_var = __create_config_var(parse_res.var_name, parse_res.var_data, parse_res.var_type);
          //__add_config_var(conf, conf_var);
          //}
          return conf;
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

  return conf;
}

static int __find_char(char* buff, char c, size_t buff_len) {
  if(buff_len <= 0)
    return -1;

  int c_index = 0;
  int found = 0;
  while(c_index <  buff_len) {
    if(buff[c_index] == c) {
      found = 1;
      break;
    }

    c_index++;
  }

  if(found)
    return c_index;
  
  return -1;
}

static void __shift_buff_left(char* buff, int start_index, int shift_amnt, size_t buff_len) {
  if(shift_amnt >= buff_len) {
    memset(buff, 0, buff_len);
    return;
  }

  for(int i = 0; i < shift_amnt; i++) {
    buff[i] = buff[start_index + i];
  }

  memset(&buff[shift_amnt], 0, buff_len - shift_amnt);
}


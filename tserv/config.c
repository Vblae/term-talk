#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "tserv/config.h"

static int __find_char(char* buff, char c, size_t buff_len);

static size_t __find_line_len(char* buff, size_t buff_len);

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

config_var_s* __create_config_var(char* line) {
  config_var_s* conf_var = (config_var_s*) malloc(sizeof(config_var_s));
  printf("line: '%s'\n", line);
  return conf_var;
}

void __add_config_var_head(config_s* conf, config_var_s* conf_var) {
  conf->vars = conf_var;
  conf->tail = conf_var;
}

void __add_config_var_tail(config_s* conf, config_var_s* conf_var) {
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
  if(!config_file_path)
    return 0;

  int fd = open(config_file_path, O_RDONLY);
  if(fd == -1)
    return 0;

  int buff_len = 1024;
  char buff[buff_len];
  memset(&buff, 0, buff_len);

  int line_num = 1;
  size_t buff_offset = 0;
  config_s* conf = create_config();
  while(1) {
    size_t bytes_read = 0;
    size_t bytes_overflowed = 0;
    while((bytes_read = read(fd, &buff[buff_offset], buff_len - buff_offset)) > 0) {
      int nl_index = 0;
      buff_offset = 0;

      int atleast_one_line = 0;
      while((nl_index = __find_char(&buff[buff_offset], '\n', buff_len - buff_offset)) != -1) {
        if(nl_index == 0 && buff_offset < buff_len) {
          buff_offset++;
          line_num++;
          continue;
        }

        int line_len = nl_index + 1;
        char line[line_len];

        memcpy(line, &buff[buff_offset], line_len);
        line[nl_index] = 0;

        config_var_s* conf_var = __create_config_var(line);
        __add_config_var(conf, conf_var);

        buff_offset += line_len;
        atleast_one_line = 1;
        line_num++;
      }
      
      // line is to long
      if(!atleast_one_line) 
        printf("line %d: error: Line is too long\n", line_num);
      
      size_t bytes_left = bytes_read + bytes_overflowed - buff_offset;
      bytes_overflowed = bytes_left;

      __shift_buff_left(buff, buff_offset, bytes_left, buff_len);
      buff_offset = bytes_left;
    }

    if(bytes_read == 0)
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

static size_t __find_line_len(char* buff, size_t buff_len) {
  int index = __find_char(buff, '\n', buff_len);
  if(index < 0)
    return buff_len;

  return index + 1;
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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "config/config.h"
#include "util/vector.h"
#include "util/log.h"

static config_var_s* __create_config_var(char* name, void* data, data_type_t type) {
  config_var_s* conf_var = (config_var_s*) malloc(sizeof(config_var_s));
  
  size_t name_len = strlen(name);
  size_t len = name_len < MAX_VAR_LEN ? name_len : MAX_VAR_LEN;
  strncpy(conf_var->name, name, len);
  conf_var->name[len] = 0;

  switch(type) {
    case BYTE_TYPE:
      conf_var->byte_val = (char) atol(data);
     break;
    case SHORT_TYPE:
      conf_var->short_val = (short) atol(data);;
      break;
    case INT_TYPE:
      conf_var->int_val = (int) atol(data);
      break;
    case LONG_TYPE:
      conf_var->long_val = atol(data);
      break;
    case FLOAT_TYPE:
      conf_var->float_val = (float) atof(data);
      break;
    case DOUBLE_TYPE:
      conf_var->double_val = atof(data);
      break;
    case STRING_TYPE:
      conf_var->string_val = strdup(data);
      break;
    default:
      break;
  }

  conf_var->type = type;
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

static void __add_config_var(config_s* conf, config_var_s* conf_var) {
  if(!conf->vars)
    __add_config_var_head(conf, conf_var);
  else
    __add_config_var_tail(conf, conf_var);
}

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

config_s* load_config(char* config_file_path) {
  if(!config_file_path) {
    m_log_error("error: invalid config file path '%s'\n", config_file_path);
    return 0;
  }

  int fd = open(config_file_path, O_RDONLY);
  if(fd == -1) {
    m_log_error("error: could not open file '%s'\n", config_file_path);
    return 0;
  }
  
  vector_s* parse_results = parse_lines(fd);
  config_s* conf = create_config();
  for(int i = 0; i < parse_results->len; i++) {
    parse_result_s* parse_res = vector_get_t(parse_result_s, parse_results, i);
    config_var_s* conf_var = __create_config_var(
      parse_res->var_name,
      parse_res->var_data,
      parse_res->var_type
    );
    __add_config_var(conf, conf_var);
    printf("config var [%p]:\n name: %s\n", conf_var, conf_var->name);
    printf(" type: %s\n", data_type_to_string(conf_var->type));
    printf(" value: ");
    switch(conf_var->type) {
      case BYTE_TYPE:
        printf("%hhi\n", conf_var->byte_val);
        break;
      case SHORT_TYPE:
        printf("%hi\n", conf_var->short_val);
        break;
      case INT_TYPE:
        printf("%d\n", conf_var->int_val);
        break;
      case LONG_TYPE:
        printf("%li\n", conf_var->long_val);
        break;
      case FLOAT_TYPE:
        printf("%f\n", conf_var->float_val);
        break;
      case DOUBLE_TYPE:
        printf("%f\n", conf_var->double_val);
        break;
      case STRING_TYPE:
        printf("%s\n", conf_var->string_val);
        break;
    }
  }
  
  vector_free(parse_results);
  return conf;
}






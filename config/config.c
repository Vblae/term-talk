#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "config/config.h"
#include "config/parse.h"
#include "util/vector.h"
#include "util/log.h"

//static config_var_s* __create_config_var(char* name, void* data, data_type_t type) {
//  config_var_s* conf_var = (config_var_s*) malloc(sizeof(config_var_s));
//  
//  size_t name_len = strlen(name);
//  size_t len = name_len < MAX_VAR_LEN ? name_len : MAX_VAR_LEN;
//  strncpy(conf_var->name, name, len);
//  conf_var->name[len] = 0;
//
//  switch(type) {
//    case BYTE_TYPE:
//      conf_var->byte_val = *((char*) data);
//     break;
//    case SHORT_TYPE:
//      conf_var->short_val = *((short*) data);
//      break;
//    case INT_TYPE:
//      conf_var->int_val = *((int*) data);
//      break;
//    case LONG_TYPE:
//      conf_var->long_val = *((long*) data);
//      break;
//    case FLOAT_TYPE:
//      conf_var->float_val = *((float*) data);
//      break;
//    case DOUBLE_TYPE:
//      conf_var->double_val = *((double*) data);
//      break;
//    case STRING_TYPE:
//      conf_var->string_val = (char*)  data;
//      break;
//    default:
//      break;
//  }
//
//  return conf_var;
//}

//static void __add_config_var_head(config_s* conf, config_var_s* conf_var) {
//  conf->vars = conf_var;
//  conf->tail = conf_var;
//}
//
//static void __add_config_var_tail(config_s* conf, config_var_s* conf_var) {
//  conf->tail->next = conf_var;
//  conf->tail = conf_var;
//}
//
//static void __add_config_var(config_s* conf, config_var_s* conf_var) {
//  if(!conf->vars)
//    __add_config_var_head(conf, conf_var);
//  else
//    __add_config_var_tail(conf, conf_var);
//}

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
  printf("GOT RESULTS\n");
  for(int i = 0; i < parse_results->len; i++) {
    parse_result_s* parse_res = (parse_result_s*) vector_get(parse_results, i);
    printf("res %d: %s => %s\n", i, parse_res->var_name, (char*) parse_res->var_data);
  }

  return conf;
}






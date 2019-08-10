#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "config/config.h"
#include "util/vector.h"
#include "util/log.h"

static config_var_s* __config_var_create(
  config_var_s* conf_var,
  char* name,
  void* data,
  data_type_t type
) {
  size_t name_len = strlen(name);
  size_t len = name_len < MAX_VAR_LEN ? name_len : MAX_VAR_LEN;
  strncpy(conf_var->name, name, len);
  conf_var->name[len] = 0;

  switch(type) {
    case INT8_TYPE:
      conf_var->int8_val = (int8_t) atol(data);
     break;
    case INT16_TYPE:
      conf_var->int16_val = (int16_t) atol(data);;
      break;
    case INT32_TYPE:
      conf_var->int32_val = (int32_t) atol(data);
      break;
    case INT64_TYPE:
      conf_var->int64_val = atol(data);
      break;
    case UINT8_TYPE:
      conf_var->uint8_val = (uint8_t) atol(data);
     break;
    case UINT16_TYPE:
      conf_var->uint16_val = (uint16_t) atol(data);;
      break;
    case UINT32_TYPE:
      conf_var->uint32_val = (uint32_t) atol(data);
      break;
    case UINT64_TYPE:
      conf_var->uint64_val = (uint64_t) atol(data);
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
  conf_var->left = 0;
  conf_var->right = 0;
  return conf_var;
}

static int32_t __add_config_var_helper(config_var_s* root, config_var_s* conf_var) {
  int32_t comparison = strcmp(conf_var->name, root->name);
  if(comparison == 0) {
    m_log_error(
      "config: error: redefined variable '%s %s'\n",
      data_type_to_string(conf_var->type),
      conf_var->name
    );
    return 0;
  }
  
  if(comparison < 0) {
    if(root->left)
      return __add_config_var_helper(root->left, conf_var);

    root->left = conf_var;
  }

  if(comparison > 0) {
    if(root->right)
      return __add_config_var_helper(root->right, conf_var);

    root->right = conf_var;
  }

  return 1;
}

static int32_t __add_config_var(config_s* conf, config_var_s* conf_var) {
  if(conf->root)
    return __add_config_var_helper(conf->root, conf_var);

  conf->root = conf_var;
  return 1;
}

static void __print_config_var(config_var_s* conf_var) {
  if(!conf_var)
    return;

  printf("config var [%p]:\n name: %s\n", conf_var, conf_var->name);
  printf(" type: %s\n", data_type_to_string(conf_var->type));
  printf(" value: ");
  switch(conf_var->type) {
    case INT8_TYPE:
      printf("%hhi\n", conf_var->int8_val);
      break;
    case INT16_TYPE:
      printf("%hi\n", conf_var->int16_val);
      break;
    case INT32_TYPE:
      printf("%d\n", conf_var->int32_val);
      break;
    case INT64_TYPE:
      printf("%lli\n", conf_var->int64_val);
      break;
    case UINT8_TYPE:
      printf("%hhu\n", conf_var->uint8_val);
      break;
    case UINT16_TYPE:
      printf("%hu\n", conf_var->uint16_val);
      break;
    case UINT32_TYPE:
      printf("%u\n", conf_var->uint32_val);
      break;
    case UINT64_TYPE:
      printf("%llu\n", conf_var->uint64_val);
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
    default:
      printf("\n");
  }

  __print_config_var(conf_var->left);
  __print_config_var(conf_var->right);
}

static void __print_config(config_s* conf) {
  if(!conf)
    return;

  __print_config_var(conf->root);
}

int32_t __vector_config_var_comparator(void* config_var0, void* config_var1) {
  return strcmp(
    (*((config_var_s**) config_var0))->name,
    (*((config_var_s**) config_var1))->name
  );
}

static void __insert_balanced_helper(
  config_s* conf,
  vector_s* conf_vars,
  int32_t lo,
  int32_t hi
) {
  if(lo > hi)
    return;
  
  int32_t mid = (hi - lo) / 2 + lo;
  config_var_s** conf_var = vector_get_of(config_var_s*, conf_vars, mid);
  __add_config_var(conf, *conf_var); 
  
  __insert_balanced_helper(conf, conf_vars, lo, mid - 1);
  __insert_balanced_helper(conf, conf_vars, mid + 1, hi);
}

static void __insert_balanced(config_s* conf, vector_s* conf_vars) {
  __insert_balanced_helper(conf, conf_vars, 0, conf_vars->len - 1);
}

config_s* config_create() {
  config_s* conf = (config_s*) malloc(sizeof(config_s));
  conf->root = 0;
  return conf;
}

void __config_var_free(config_var_s* conf_var) {
  if(!conf_var)
    return;

  if(conf_var->type == STRING_TYPE)
    free(conf_var->string_val);

  __config_var_free(conf_var->left);
  __config_var_free(conf_var->right);
}
void config_free(config_s* conf) {
  __config_var_free(conf->root);
  free(conf->root);
  free(conf);
}

config_s* load_config(char* config_file_path) {
  if(!config_file_path) {
    m_log_error("error: invalid config file path '%s'\n", config_file_path);
    return 0;
  }

  int32_t fd = open(config_file_path, O_RDONLY);
  if(fd == -1) {
    m_log_error("error: could not open file '%s'\n", config_file_path);
    return 0;
  }
  
  vector_s* parse_results = parse_lines(fd);
  config_s* conf = config_create();
  if(!parse_results)
    return 0;

  if(!conf) {
    printf("error: failed to allocate memory for config\n");
    vector_free(parse_results);
    return 0;
  }

  if(parse_results->len == 0) {
    vector_free(parse_results);
    return conf;
  }

   config_var_s* config_var_block =
     (config_var_s*) malloc(parse_results->len * sizeof(config_var_s));

  if(!config_var_block) {
    printf("error: failed to allocate memory for config var block\n");
    vector_free(parse_results);
    config_free(conf);
    return 0;
  }

  vector_s* conf_vars = vector_create(parse_results->len, sizeof(config_var_s*));
  if(!conf_vars) {
    printf("error: failes to allocate memory for config var vector\n");
    vector_free(parse_results);
    config_free(conf);
    return 0;
  }

  for(int32_t i = 0; i < parse_results->len; i++) {
    parse_result_s* parse_res = vector_get_of(parse_result_s, parse_results, i);
    config_var_s* conf_var = __config_var_create(
      &config_var_block[i],
      parse_res->var_name,
      parse_res->var_data,
      parse_res->var_type
    );

    vector_push(conf_vars, &conf_var);
  }

  vector_sort(conf_vars, __vector_config_var_comparator);
  __insert_balanced(conf, conf_vars);
  __print_config(conf);
  vector_free(parse_results);
  return conf;
}


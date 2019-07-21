#include <stdlib.h>
#include <stdio.h>

#include "util/vector.h"

vector_s* create_vector(
    size_t reserve,
    size_t item_size,
    vector_item_comparator_f comparator_funct
) {
  if(!item_size) {
    printf("error: vector: item_size cannot be zero\n");
    return 0;
  }

  if(!comparator_funct) {
    printf("error: vector: comparator_funct cannot be null\n");
    return 0;
  }

  size_t actual_reserve = reserve ? reserve : VECTOR_DEFAULT_RESERVE;

  vector_s* vector = (vector_s*) malloc(sizeof(vector_s));
  vector->data = malloc(actual_reserve * sizeof(void*));
  vector->__data_block = malloc(actual_reserve * item_size);
  vector->len = 0;
  vector->cap = actual_reserve;
  return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util/vector.h"

struct vector_wrapper {
  vector_s vec;

  size_t __item_size;
  void* __data_block;

  vector_item_comparator_f __comparator_funct;
  vector_item_allocator_f __allocator_funct;
  vector_item_deallocator_f __deallocator_funct;
};

typedef struct vector_wrapper vector_wrapper_s;

static int  __vector_default_allocator(void* item) {
  return 1;
}

static void __vector_default_deallocator(void* item) {

}

static int __vector_byte_comparator(void* item1, void* item2) {
  return *((char*) item1) - *((char*) item2);
}

static int __vector_short_comparator(void* item1, void* item2) {
  return *((short*) item1) - *((short*) item2);
}

static int __vector_int_comparator(void* item1, void* item2) {
  return *((int*) item1) - *((int*) item2);
}

static int __vector_long_comparator(void* item1, void* item2) {
  return *((long*) item1) - *((int*) item2);
}

static int __vector_float_comparator(void* item1, void* item2) {
  return *((float*) item1) - *((float*) item2);
}

static int __vector_double_comparator(void* item1, void* item2) {
  return *((double*) item1) - *((double*) item2);
}

static int __vector_pointer_comparator(void* item1, void* item2) {
  return item1 - item2;
}

int __vector_undo_double_capacity(
  vector_wrapper_s* vector,
  size_t num_allocated,
  void* new_data_block
) {
  if(num_allocated == 0)
    return 0;

  for(int i = 0; i < num_allocated; i++) {
    vector->__deallocator_funct(&new_data_block[i * vector->__item_size]);
  }

  free(new_data_block);
  return 0;
}

int __vector_double_capacity(vector_wrapper_s* vector) {
  void* new_data_block = malloc(2 * vector->vec.cap * vector->__item_size);
  void* old_data_block = vector->__data_block;
  if(!new_data_block)
    return 0;
  
  int success = 1;
  size_t num_allocated = 0;
  for(int i = 0; i < vector->vec.len; i++) {
    void* new_item_spot = &new_data_block[i * vector->__item_size];
    void* old_item_spot = &old_data_block[i * vector->__item_size];

    memcpy(new_item_spot, old_item_spot, vector->__item_size);
    success = vector->__allocator_funct(new_item_spot);
    if(!success)
      break;

    num_allocated++;
  }

  if(!success)
    return __vector_undo_double_capacity(vector, num_allocated, new_data_block);
  
  vector->__data_block = new_data_block;
  vector->vec.cap *= 2;

  for(int i = vector->vec.len; i < vector->vec.cap; i++) {
    memset(&vector->__data_block[i * vector->__item_size], 0, vector->__item_size);
  }

  free(old_data_block);
  return success;
}

vector_s* vector_create_with_allocators(
    size_t reserve,
    size_t item_size,
    vector_item_comparator_f comparator_funct,
    vector_item_allocator_f allocator_funct,
    vector_item_deallocator_f deallocator_funct
) {
  if(!item_size) {
    printf("error: vector: item_size cannot be zero\n");
    return 0;
  }

  if(!comparator_funct) {
    printf("error: vector: comparator_funct cannot be null\n");
    return 0;
  }

  if(!allocator_funct)
    allocator_funct = __vector_default_allocator;

  if(!deallocator_funct)
    deallocator_funct = __vector_default_deallocator;

  size_t actual_reserve = reserve ? reserve : VECTOR_DEFAULT_RESERVE;

  vector_wrapper_s* vector = (vector_wrapper_s*) malloc(sizeof(vector_wrapper_s));
  if(!vector) {
    printf("error: vector: failed to malloc vector memory\n");
    return 0;
  }

  vector->__data_block = malloc(actual_reserve * item_size);
  if(!vector->__data_block) {
    free(vector);
    printf("error: vector: failed to malloc vector data block\n");
    return 0;
  }

  vector->vec.len = 0;
  vector->vec.cap = actual_reserve;

  vector->__item_size = item_size;
  vector->__comparator_funct = comparator_funct;
  vector->__allocator_funct = allocator_funct;
  vector->__deallocator_funct = deallocator_funct;
  return &vector->vec;
}

vector_s* vector_create(
  size_t reserve,
  size_t item_size,
  vector_item_comparator_f comparator_funct
) {
  return vector_create_with_allocators(
      reserve,
      item_size,
      comparator_funct,
      &__vector_default_allocator,
      &__vector_default_deallocator
  );
}

vector_s* vector_of_byte_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(char),
    &__vector_byte_comparator,
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_short_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(short),
    &__vector_short_comparator,
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_int_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(int),
    &__vector_int_comparator,
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_long_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(long),
    &__vector_long_comparator,
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_pointer_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(void*),
    &__vector_pointer_comparator,
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_float_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(float),
    &__vector_float_comparator,
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_double_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(double),
    &__vector_double_comparator,
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

void vector_free(vector_s* vector) {
  if(!vector)
    return;
  
  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  for(int i = 0; i < vector_wrapper->vec.len; i++) {
    vector_wrapper->__deallocator_funct(  
      &vector_wrapper->__data_block[i * vector_wrapper->__item_size]
    );
  }

  free(vector_wrapper->__data_block);
}

int vector_push(vector_s* vector, void* item) {
  if(!vector) {
    printf("error: vector: cannot push to null vector\n");
    return 0;
  }

  if(!item) {
    printf("error: vector: cannot push null item\n");
    return 0;
  }

  if(
      vector->len == vector->cap &&
      !__vector_double_capacity((vector_wrapper_s*) vector)
  ) {
    printf("error: vector: failed to increase vector capacity\n");
    return 0;
  }

  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  void* free_item_spot =
    &vector_wrapper->__data_block[vector->len * vector_wrapper->__item_size];

  memcpy(free_item_spot, item, vector_wrapper->__item_size);
  int success = vector_wrapper->__allocator_funct(free_item_spot);
  if(success)
    vector->len++;
  else
    memset(free_item_spot, 0, vector_wrapper->__item_size);

  return success;
}

void* vector_get(vector_s* vector, int idx) {
  if(!vector)
    return 0;

  if(idx < 0 || idx > vector->len)
    return 0;

  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  return &vector_wrapper->__data_block[idx * vector_wrapper->__item_size];
}


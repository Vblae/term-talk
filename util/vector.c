#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util/log.h"
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

static int32_t  __vector_default_allocator(void* item) {
  return 1;
}

static void __vector_default_deallocator(void* item) {

}

int32_t __vector_double_capacity(vector_wrapper_s* vector) {
  void* new_data_block = malloc(2 * vector->vec.cap * vector->__item_size);
  void* old_data_block = vector->__data_block;
  if(!new_data_block)
    return 0;

  memcpy(new_data_block, old_data_block, vector->vec.len * vector->__item_size);
  vector->__data_block = new_data_block;
  vector->vec.cap *= 2;

  for(int32_t i = vector->vec.len; i < vector->vec.cap; i++) {
    memset(&vector->__data_block[i * vector->__item_size], 0, vector->__item_size);
  }

  free(old_data_block);
  return 1;
}

int32_t vector_byte_comparator(void* item1, void* item2) {
  return *((char*) item1) - *((char*) item2);
}

int32_t vector_short_comparator(void* item1, void* item2) {
  return *((short*) item1) - *((short*) item2);
}

int32_t vector_int_comparator(void* item1, void* item2) {
  return *((int32_t*) item1) - *((int*) item2);
}

int32_t vector_long_comparator(void* item1, void* item2) {
  return *((long*) item1) - *((int32_t*) item2);
}

int32_t vector_float_comparator(void* item1, void* item2) {
  return *((float*) item1) - *((float*) item2);
}

int32_t vector_double_comparator(void* item1, void* item2) {
  return *((double*) item1) - *((double*) item2);
}

int32_t vector_pointer_comparator(void* item1, void* item2) {
  return item1 - item2;
}

int32_t vector_string_comparator(void* item1, void* item2) {
  if(!*((char**) item1) && !*((char**) item2))
      return 0;

  if(!*((char**) item1))
    return 1;

  if(!*((char**) item2))
    return -1;

  return strcmp(*((char**) item1), *((char**) item2));
}

vector_s* vector_create_with_allocators(
    size_t reserve,
    size_t item_size,
    vector_item_allocator_f allocator_funct,
    vector_item_deallocator_f deallocator_funct
) {
  if(!item_size) {
    LOGE("vector: error: item_size cannot be zero\n");
    return NULL;
  }

  if(!allocator_funct)
    allocator_funct = __vector_default_allocator;

  if(!deallocator_funct)
    deallocator_funct = __vector_default_deallocator;

  size_t actual_reserve = reserve ? reserve : VECTOR_DEFAULT_RESERVE;

  vector_wrapper_s* vector = (vector_wrapper_s*) malloc(sizeof(vector_wrapper_s));
  if(!vector) {
    LOGE("vector: error: failed to malloc vector memory\n");
    return NULL;
  }

  vector->__data_block = malloc(actual_reserve * item_size);
  if(!vector->__data_block) {
    free(vector);
    LOGE("vector: error: failed to malloc vector data block\n");
    return NULL;
  }

  vector->vec.len = 0;
  vector->vec.cap = actual_reserve;

  vector->__item_size = item_size;
  vector->__allocator_funct = allocator_funct;
  vector->__deallocator_funct = deallocator_funct;
  return &vector->vec;
}

vector_s* vector_create(
  size_t reserve,
  size_t item_size
) {
  return vector_create_with_allocators(
      reserve,
      item_size,
      &__vector_default_allocator,
      &__vector_default_deallocator
  );
}

vector_s* vector_of_byte_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(int8_t),
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_short_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(int16_t),
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_int_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(int32_t),
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_long_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(int64_t),
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_pointer_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(void*),
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_float_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(float),
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_double_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(double),
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

vector_s* vector_of_string_create(size_t reserve) {
  return vector_create_with_allocators(
    reserve,
    sizeof(char*),
    &__vector_default_allocator,
    &__vector_default_deallocator
  );
}

void vector_free(vector_s* vector) {
  if(!vector)
    return;
  
  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  for(int32_t i = 0; i < vector_wrapper->vec.len; i++) {
    vector_wrapper->__deallocator_funct(  
      &vector_wrapper->__data_block[i * vector_wrapper->__item_size]
    );
  }

  free(vector_wrapper->__data_block);
}

int32_t vector_push(vector_s* vector, void* item) {
  if(!vector) {
    LOGE("vector: error: cannot push to null vector\n");
    return 0;
  }

  if(!item) {
    LOGE("vector: error: cannot push null item\n");
    return 0;
  }

  if(
      vector->len == vector->cap &&
      !__vector_double_capacity((vector_wrapper_s*) vector)
  ) {
    LOGE("vector: error: failed to increase vector capacity\n");
    return 0;
  }

  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  void* free_item_spot =
    &vector_wrapper->__data_block[vector->len * vector_wrapper->__item_size];

  memcpy(free_item_spot, item, vector_wrapper->__item_size);
  int32_t success = vector_wrapper->__allocator_funct(free_item_spot);
  if(success)
    vector->len++;
  else
    memset(free_item_spot, 0, vector_wrapper->__item_size);

  return success;
}

int32_t vector_pop(vector_s* vector) {
  if(!vector) {
    LOGE("vector: error: cannot pop from a null vector\n");
    return 0;
  }

  if(vector->len == 0)
    return 0;

  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  memset(
    &vector_wrapper->__data_block[(vector->len - 1) * vector_wrapper->__item_size],
    0,
    vector_wrapper->__item_size
  );
  
  vector->len--;
  return 1;
}

void* vector_back(vector_s* vector) {
  if(!vector) {
    LOGE("vector: error: cannot get top from a null vector\n");
    return NULL;
  }

  if(vector->len == 0)
    return NULL;

  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  return &vector_wrapper->__data_block[(vector->len - 1) * vector_wrapper->__item_size];
}

void* vector_get(vector_s* vector, int32_t idx) {
  if(!vector)
    return NULL;

  if(idx < 0 || idx > vector->len)
    return NULL;

  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  return &vector_wrapper->__data_block[idx * vector_wrapper->__item_size];
}

int32_t vector_find(vector_s* vector, void* val, vector_item_comparator_f comp_funct) {
  if(!vector || !val)
    return -1;
  
  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  for(int32_t i = 0; i < vector->len; i++) {
    void* other = &vector_wrapper->__data_block[i * vector_wrapper->__item_size];
    if(comp_funct(val, other) == 0)
      return i;
  }

  return -1;
}

void vector_clear(vector_s* vector) {
  if(!vector || !vector->len)
    return;

  vector_wrapper_s* vector_wrapper = (vector_wrapper_s*) vector;
  memset(vector_wrapper->__data_block, 0, vector->len * vector_wrapper->__item_size);
  vector->len = 0;
}

static void __vector_swap(vector_wrapper_s* vector, int32_t i, int32_t j) {
  char temp[vector->__item_size];
  void* ptr_i = &vector->__data_block[i * vector->__item_size];
  void* ptr_j = &vector->__data_block[j * vector->__item_size];

  memcpy(temp, ptr_i, vector->__item_size);
  memcpy(ptr_i, ptr_j, vector->__item_size);
  memcpy(ptr_j, temp, vector->__item_size);
}

static int32_t __vector_quick_sort_partition(
  vector_wrapper_s* vector,
  int32_t lo,
  int32_t hi,
  vector_item_comparator_f comp_funct
) {
  void* pivot = &vector->__data_block[hi * vector->__item_size];
  int32_t i = lo - 1;
  for(int32_t j = lo; j <= hi - 1; j++) {
    void* other = &vector->__data_block[j * vector->__item_size];
    if(comp_funct(other, pivot) <= 0)
      __vector_swap(vector, ++i, j);
  }

  __vector_swap(vector, i + 1, hi);
  return i + 1;
}

static inline void __vector_quick_sort(
  vector_wrapper_s* vector,
  int32_t lo,
  int32_t hi,
  vector_item_comparator_f comp_funct
) {
  if(lo < hi) {
    int32_t pivot = __vector_quick_sort_partition(vector, lo, hi, comp_funct);
    __vector_quick_sort(vector, lo, pivot - 1, comp_funct);
    __vector_quick_sort(vector, pivot + 1, hi, comp_funct);
  }
}

static inline void __vector_sort(
  vector_wrapper_s* vector,
  vector_item_comparator_f comp_funct
) {
  __vector_quick_sort(vector, 0, vector->vec.len - 1, comp_funct);
}

void vector_sort(vector_s* vector, vector_item_comparator_f comp_funct) {
  __vector_sort((vector_wrapper_s*) vector, comp_funct);
}


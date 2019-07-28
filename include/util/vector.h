#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stddef.h>

#define VECTOR_DEFAULT_RESERVE 0x08

typedef int (*vector_item_comparator_f)(void*, void*);

typedef int (*vector_item_allocator_f)(void*);

typedef void (*vector_item_deallocator_f)(void*);

typedef void* (*vector_item_getter_f)(size_t);

struct vector {
  size_t len;
  size_t cap;
};

typedef struct vector vector_s;

vector_s* vector_create_with_allocators(
  size_t reserve,
  size_t item_size,
  vector_item_comparator_f comparator_funct,
  vector_item_allocator_f allocator_funct,
  vector_item_deallocator_f deallocator_funct
);

vector_s* vector_create(
  size_t reserve,
  size_t item_size,
  vector_item_comparator_f comparator_funct
);

vector_s* vector_of_byte_create(size_t reserve);

vector_s* vector_of_short_create(size_t reserve);

vector_s* vector_of_int_create(size_t reserve);

vector_s* vector_of_long_create(size_t reserve);

vector_s* vector_of_pointer_create(size_t reserve);

vector_s* vector_of_float_create(size_t reserve);

vector_s* vector_of_double_create(size_t reserve);

vector_s* vector_of_string_create(size_t reserve);

void vector_free(vector_s* vector);

int vector_push(vector_s* vector, void* item);

int vector_pop(vector_s* vector);

void* vector_top(vector_s* vector);

void* vector_get(vector_s* vector, int idx);

int vector_find(vector_s* vector, void* val);

void vector_clear(vector_s* vector);

#endif


#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stddef.h>

#define VECTOR_DEFAULT_RESERVE 0x08

typedef void* (*vector_item_allocator_f)(void*);
typedef int (*vector_item_comparator_f)(void*, void*);

typedef void* (*vector_item_getter_f)(size_t);

struct vector {
  size_t len;
  size_t cap;

  void* data;
  void* __data_block;

  vector_item_comparator_f comparator_funct;
  vector_item_getter_f get;
};

typedef struct vector vector_s;

void init_vector(vector_s* vector, vector_item_comparator_f comparator_funct);

vector_s* create_vector(
  size_t reserve,
  size_t item_size,
  vector_item_comparator_f comparator_funct
);

#endif


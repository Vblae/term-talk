#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stddef.h>
#include <stdint.h>

#define VECTOR_DEFAULT_RESERVE 0x08

#define vector_create_of(type, reserve) vector_create((reserve), sizeof(type))
#define vector_back_of(type, vector) ((type*) vector_back((vector)))
#define vector_get_of(type, vector, idx) ((type*) vector_get((vector), (idx)))

typedef int32_t (*vector_item_comparator_f)(void*, void*);

typedef int32_t (*vector_item_allocator_f)(void*);

typedef void (*vector_item_deallocator_f)(void*);

typedef void* (*vector_item_getter_f)(size_t);

struct vector {
  size_t len;
  size_t cap;
};

typedef struct vector vector_s;

int32_t vector_byte_comparator(void* item1, void* item2);

int32_t vector_short_comparator(void* item1, void* item2);

int32_t vector_int_comparator(void* item1, void* item2);

int32_t vector_long_comparator(void* item1, void* item2);

int32_t vector_float_comparator(void* item1, void* item2);

int32_t vector_double_comparator(void* item1, void* item2);

int32_t vector_pointer_comparator(void* item1, void* item2);

int32_t vector_string_comparator(void* item1, void* item2);

vector_s* vector_create_with_allocators(
  size_t reserve,
  size_t item_size,
  vector_item_allocator_f allocator_funct,
  vector_item_deallocator_f deallocator_funct
);

vector_s* vector_create(
  size_t reserve,
  size_t item_size
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

int32_t vector_push(vector_s* vector, void* item);

int32_t vector_pop(vector_s* vector);

void* vector_back(vector_s* vector);

void* vector_get(vector_s* vector, int32_t idx);

int32_t vector_find(vector_s* vector, void* val, vector_item_comparator_f comp_funct);

void vector_clear(vector_s* vector);

void vector_sort(vector_s* vector, vector_item_comparator_f comp_funct);

#endif


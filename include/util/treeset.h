#ifndef __TREE_SET_H__
#define __TREE_SET_H__

#include <stddef.h>
#include <stdint.h>

#define tree_set_create_of(key_type, comp_funct) \
  tree_set_create(sizeof(key_type), (comp_funct))

typedef int32_t (*tree_set_key_comparator_f)(void*, void*);

typedef int32_t (*tree_set_key_allocator_f)(void*, void*);
typedef void (*tree_set_key_deallocator_f)(void*);

typedef struct tree_set {
  size_t size;
} tree_set_s;

int32_t tree_set_int_key_comparator(void* key0, void* key1);

int32_t tree_set_string_key_comparator(void* key0, void* key1);

int32_t tree_set_string_key_allocator(void* src, void* dst);

void tree_set_string_key_deallocator(void* ptr);

tree_set_s* tree_set_create_with_allocators(
  size_t key_size,
  tree_set_key_comparator_f key_comp,
  tree_set_key_allocator_f key_allocator,
  tree_set_key_deallocator_f key_deallocator
);

tree_set_s* tree_set_create_of_string_key();

tree_set_s* tree_set_create_of_int_key();

tree_set_s* tree_set_create(
  size_t key_size,
  tree_set_key_comparator_f key_comp)
;

void tree_set_free(tree_set_s* set);

int32_t tree_set_insert(tree_set_s* set, void* key);

int32_t tree_set_delete(tree_set_s* set, void* key);

int32_t tree_set_contains(tree_set_s* set, void* key);

void tree_set_print(
  tree_set_s* set,
  void (*key_printer)(void*),
  void (*val_printer)(void*)
);
 
#endif


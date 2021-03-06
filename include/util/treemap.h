#ifndef __TREE_MAP_H__
#define __TREE_MAP_H__

#include <stdint.h>
#include <stddef.h>

#define tree_map_create_of(key_type, val_type, comp_funct) \
  tree_map_create(sizeof(key_type), sizeof(val_type), (comp_funct))

#define tree_map_get_of(type, map, key) ((type*) tree_map_get((map), (key)))

typedef int32_t (*tree_map_key_comparator_f)(void*, void*);
typedef int32_t (*tree_map_val_comparator_f)(void*, void*);

typedef int32_t (*tree_map_key_allocator_f)(void*, void*);
typedef void (*tree_map_key_deallocator_f)(void*);

typedef struct tree_map {
  size_t size;
} tree_map_s;

int32_t tree_map_int_key_comparator(void* key0, void* key1); 

int32_t tree_map_string_key_comparator(void* key0, void* key1);

int32_t tree_map_string_key_allocator(void* src, void* dst);

void tree_map_string_key_deallocator(void* ptr);

tree_map_s* tree_map_create_with_allocators(
  size_t key_size,
  size_t val_size,
  tree_map_key_comparator_f key_comp,
  tree_map_key_allocator_f key_allocator,
  tree_map_key_deallocator_f key_deallocator
);

tree_map_s* tree_map_create_of_string_key(size_t val_size);

tree_map_s* tree_map_create_of_int_key(size_t val_size);

tree_map_s* tree_map_create(
  size_t key_size,
  size_t val_size,
  tree_map_key_comparator_f key_comp
);

void tree_map_free(tree_map_s* map);

void* tree_map_get(tree_map_s* map, void* key);

int32_t tree_map_insert(tree_map_s* map, void* key, void* val);

int32_t tree_map_delete(tree_map_s* map, void* key);


void tree_map_print(
  tree_map_s* map,
  void (*key_printer)(void*),
  void (*val_printer)(void*)
);
 
#endif


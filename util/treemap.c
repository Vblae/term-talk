#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/treemap.h"
#include "util/log.h"

#define AS_WRAPPER(map) ((tree_map_wrapper_s*) (map))

typedef struct tree_map_node {
  void* key;
  void* val;

  struct tree_map_node* left;
  struct tree_map_node* right;
} tree_map_node_s;

typedef struct tree_map_wrapper {
  tree_map_s map;

  size_t __key_size;
  size_t __val_size;

  tree_map_node_s* __root;
  tree_map_key_comparator_f __key_comp;
  tree_map_key_allocator_f __key_allocator;
  tree_map_key_deallocator_f __key_deallocator;
} tree_map_wrapper_s;

int32_t tree_map_int_key_comparator(void* key0, void* key1) {
  return *((int*) key0) - *((int*) key1);
}

int32_t tree_map_string_key_comparator(void* key0, void* key1) {
  return strcmp(*((char**) key0), *((char**) key1));
}

int32_t tree_map_string_key_allocator(void* src, void* dst) {
  char* key_copy = strdup(*((char**) src));
  if(!key_copy) {
    LOGE(
      "treemap: error: failed to allocate mem tree_map_string_key_allocator()\n"
    );
    return 0;
  }

  memcpy(dst, &key_copy, sizeof(char*));
  return 1;
}

void tree_map_string_key_deallocator(void* ptr) {
  free(*((char**) ptr));
}

static tree_map_node_s* __tree_map_node_create(
  tree_map_wrapper_s* map,
  void* key,
  void* val
) {
  tree_map_node_s* node = malloc(sizeof(tree_map_node_s));
  if(!node) {
    LOGE("treemap: error: failed to allocate mem for tree map node\n");
    return 0;
  }

  void* key_space = malloc(map->__key_size);
  if(!key_space) {
    LOGE("treemap: error: failed to allocate mem for key space\n");
    free(node);
    return NULL;
  }

  void* val_space = malloc(map->__val_size);
  if(!val_space) {
    LOGE("treemap: error: failed to allocate mem for val space\n");
    free(key_space);
    free(node);
    return NULL;
  }

  int32_t key_allocation_succeeded;
  if(map->__key_allocator) {
    key_allocation_succeeded = map->__key_allocator(key, key_space);
  } else {
    memcpy(key_space, key, map->__key_size);
    key_allocation_succeeded = 1;
  }

  if(!key_allocation_succeeded) {
    free(key_space);
    free(node);
    return NULL;
  }

  memcpy(val_space, val, map->__val_size);
  node->key = key_space;
  node->val = val_space;
  node->left = 0;
  node->right = 0;
  return node;
}

tree_map_s* tree_map_create_with_allocators(
  size_t key_size,
  size_t val_size,
  tree_map_key_comparator_f key_comp,
  tree_map_key_allocator_f key_allocator,
  tree_map_key_deallocator_f key_deallocator
) {
  if(!key_size) {
    LOGE("treemap: error: key_size cannot be zero\n");
    return NULL;
  }

  if(!val_size) {
    LOGE("treemap: error: val_size cannot be zero\n");
    return NULL;
  }

  if(!key_comp) {
    LOGE("treemap: error: key comparator function cannot be null\n");
    return NULL;
  }

  tree_map_wrapper_s* tree_map =  malloc(sizeof(tree_map_wrapper_s));
  if(!tree_map) {
    LOGE("treemap: error: failed to allocate mem for tree_map\n");
    return NULL;
  }

  tree_map->__key_size = key_size;
  tree_map->__val_size = val_size;
  tree_map->__key_comp = key_comp;
  tree_map->__key_allocator = key_allocator;
  tree_map->__key_deallocator = key_deallocator;
  tree_map->__root = 0;
  tree_map->map.size = 0;
  return &tree_map->map;
}

tree_map_s* tree_map_create_of_string_key(size_t val_size) {
  return tree_map_create_with_allocators(
    sizeof(char*),
    val_size,
    &tree_map_string_key_comparator,
    &tree_map_string_key_allocator,
    &tree_map_string_key_deallocator
  );
}

tree_map_s* tree_map_create_of_int_key(size_t val_size) {
  return tree_map_create_with_allocators(
    sizeof(int32_t),
    val_size,
    &tree_map_int_key_comparator,
    NULL,
    NULL
  );
}

tree_map_s* tree_map_create(
  size_t key_size,
  size_t val_size,
  tree_map_key_comparator_f key_comp
) {
  return tree_map_create_with_allocators(key_size, val_size, key_comp, 0, 0);
}

static void __tree_map_node_free(tree_map_wrapper_s* map, tree_map_node_s* root) {
  if(!map || !root)
    return;

  __tree_map_node_free(map, root->left);
  __tree_map_node_free(map, root->right);

  if(map->__key_deallocator)
    map->__key_deallocator(root->key);

  free(root->key);
  free(root->val);
  free(root);
}

static void __tree_map_free(tree_map_wrapper_s* map) {
  if(!map)
    return;

  __tree_map_node_free(map, map->__root);
  free(map);
}

void tree_map_free(tree_map_s* map) {
  __tree_map_free(AS_WRAPPER(map));
}

static int32_t __tree_map_insert_helper(
  tree_map_wrapper_s* map,
  tree_map_node_s* root,
  void* key,
  void* val
) {
  int comparison = map->__key_comp(key, root->key);
  if(comparison == 0) {
    memcpy(root->val, val, map->__val_size);
    return 1;
  } else if(comparison < 0) {
    if(root->left)
      return __tree_map_insert_helper(map, root->left, key, val);

    root->left = __tree_map_node_create(map, key, val);
    if(root->left)
      return 1;

    return 0;
  } else {
    if(root->right)
      return __tree_map_insert_helper(map, root->right, key, val);

    root->right = __tree_map_node_create(map, key, val);
    if(root->right)
      return 1;

    return 0;
  }
}

static int32_t __tree_map_insert(tree_map_wrapper_s* map, void* key, void* val) {
  if(map->__root)
    return __tree_map_insert_helper(map, map->__root, key, val);

  map->__root = __tree_map_node_create(map, key, val);
  return map->__root != 0;
}

int32_t tree_map_insert(tree_map_s* map, void* key, void* val) {
  if(!map) {
    LOGE("treemap: error: cannot insert into null map\n");
    return 0;
  }

  if(!key) {
    LOGE("treemap: error:cannot insert null key into map\n");
    return 0;
  }

  int32_t inserted = __tree_map_insert(AS_WRAPPER(map), key, val);
  if(inserted)
    map->size++;

  return inserted;
}

static void* __tree_map_get_helper(
  tree_map_wrapper_s* map,
  tree_map_node_s* root,
  void* key
) {
  if(!root)
    return 0;

  int32_t comparison = map->__key_comp(key, root->key);
  if(comparison == 0)
    return root->val;

  if(comparison < 0)
    return __tree_map_get_helper(map, root->left, key);

  return __tree_map_get_helper(map, root->right, key);
}

static void* __tree_map_get(tree_map_wrapper_s* map, void* key) {
  return __tree_map_get_helper(map, map->__root, key);
}

void* tree_map_get(tree_map_s* map, void* key) {
  if(!map) {
    LOGE("treemap: error: cannot perform find in null map\n");
    return 0;
  }

  if(!key) {
    LOGE("treemap: error: cannot perform find on null key\n");
    return 0;
  }

  return __tree_map_get(AS_WRAPPER(map), key);
}

void __tree_map_print_helper(
  tree_map_wrapper_s* map,
  tree_map_node_s* root,
  void (*key_printer)(void*),
  void (*val_printer)(void*)
) {
  if(!root)
    return;

  __tree_map_print_helper(map, root->left, key_printer, val_printer);

  printf("node key*:%p ", root->key);
  key_printer(root->key);
  printf(" val*:%p ", root->val);
  val_printer(root->val);
  printf("\n");

  __tree_map_print_helper(map, root->right, key_printer, val_printer);
}
void __tree_map_print(
  tree_map_wrapper_s* map,
  void (*key_printer)(void*),
  void (*val_printer)(void*)
) {
  return __tree_map_print_helper(map, map->__root, key_printer, val_printer);
}

void tree_map_print(
  tree_map_s* map,
  void (*key_printer)(void*),
  void (*val_printer)(void*)
) {
  return __tree_map_print(AS_WRAPPER(map), key_printer, val_printer);
}


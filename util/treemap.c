#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/treemap.h"
#include "util/log.h"

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

int32_t tree_map_string_key_comparator(void* key0, void* key1) {
  return strcmp(*((char**) key0), *((char**) key1));
}

int32_t tree_map_string_key_allocator(void* src, void* dst) {
  char* key_copy = strdup(*((char**) src));
  if(!key_copy) {
    m_log_error(
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
  tree_map_node_s* node = (tree_map_node_s*) malloc(sizeof(tree_map_node_s));
  if(!node) {
    m_log_error("treemap: error: failed to allocate mem for tree map node\n");
    return 0;
  }

  void* key_space = malloc(map->__key_size);
  void* val_space = malloc(map->__val_size);
  if(!key_space) {
    m_log_error("treemap: error: failed to allocate mem for key space\n");
    free(node);
    return 0;
  }

  if(!key_space) {
    m_log_error("treemap: error: failed to allocate mem for val space\n");
    free(key_space);
    free(node);
    return 0;
  }

  if(map->__key_allocator) {
    map->__key_allocator(key, key_space);
  } else {
    memcpy(key_space, key, map->__key_size);
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
    m_log_error("treemap: error: key_size can not be zero\n");
    return 0;
  }
  
  if(!val_size) {
    m_log_error("treemap: error: val_size can not be zeror\n");
    return 0;
  }

  if(!key_comp) {
    m_log_error("treemap: error: key comparator function can not be null\n");
    return 0;
  }

  tree_map_wrapper_s* tree_map =
    (tree_map_wrapper_s*) malloc(sizeof(tree_map_wrapper_s));
  
  if(!tree_map) {
    m_log_error("treemap: error: failed to allocate mem for tree_map\n");
    return 0;
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

tree_map_s* tree_map_create_of_string_key(
  size_t val_size
) {
  return tree_map_create_with_allocators(
    sizeof(char*),
    val_size,
    &tree_map_string_key_comparator,
    &tree_map_string_key_allocator,
    &tree_map_string_key_deallocator
  );
}

tree_map_s* tree_map_create(
  size_t key_size,
  size_t val_size,
  tree_map_key_comparator_f key_comp
) {
  return tree_map_create_with_allocators(
    key_size,
    val_size,
    key_comp,
    0,
    0
  );
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

void tree_map_free(tree_map_s* map) {
  if(!map)
    return;
  
  tree_map_wrapper_s* map_wrapper = (tree_map_wrapper_s*) map;
  __tree_map_node_free(map_wrapper, map_wrapper->__root);
  free(map_wrapper);
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
  }else if(comparison < 0) {
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
  if(map->__root)
    return 1;

  return 0;
}

int32_t tree_map_insert(tree_map_s* map, void* key, void* val) {
  if(!map) {
    m_log_error("treemap: error: can not insert into null map\n");
    return 0;
  }

  if(!key) {
    m_log_error("treemap: error: insert key can not be null\n");
    return 0;
  }

  return __tree_map_insert((tree_map_wrapper_s*) map, key, val);
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
    m_log_error("treemap: error: can not perform find in null map\n");
    return 0;
  }

  if(!key) {
    m_log_error("treemap: error: can not perform find on null key\n");
    return 0;
  }

  return __tree_map_get((tree_map_wrapper_s*) map, key);
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
  return __tree_map_print((tree_map_wrapper_s*) map, key_printer, val_printer);
}


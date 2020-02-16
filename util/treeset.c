#include <stdlib.h>
#include <string.h>

#include "util/treeset.h"
#include "util/log.h"


typedef struct tree_set_node {
  void* key;

  struct tree_set_node* left;
  struct tree_set_node* right;
} tree_set_node_s;

typedef struct tree_set_wrapper {
  tree_set_s set;

  size_t __key_size;
  tree_set_node_s* __root;
  tree_set_key_comparator_f __key_comp;
  tree_set_key_allocator_f __key_allocator;
  tree_set_key_deallocator_f __key_deallocator;
} tree_set_wrapper_s;

int32_t tree_set_int_key_comparator(void* key0, void* key1) {
  return *((int*) key0) - *((int*) key1);
}

int32_t tree_set_string_key_comparator(void* key0, void* key1) {
  return strcmp(*((char**) key0), *((char**) key1));
}

int32_t tree_set_string_key_allocator(void* src, void* dst) {
  char* key_copy = strdup(*((char**) src));
  if(!key_copy) {
    LOGE(
      "treeset: error: failed to allocate mem tree_set_string_key_allocator()\n"
    );
    return 0;
  }

  memcpy(dst, &key_copy, sizeof(char*));
  return 1;
}

void tree_set_string_key_deallocator(void* ptr) {
  free(*((char**) ptr));
}

static tree_set_node_s* __tree_set_node_create(tree_set_wrapper_s* set, void* key) {
  tree_set_node_s* node = (tree_set_node_s*) malloc(sizeof(tree_set_node_s));
  if(!node) {
    LOGE("treeset: error: failed to allocate memory for tree set node\n");
    return NULL;
  }

  void* key_space = malloc(set->__key_size);
  if(!key_space) {
    LOGE("treeset: error: failed to allocate memory for tree set node key\n");
    free(node);
    return NULL;
  }
  
  int32_t key_allocation_succeeded;
  if(set->__key_allocator) {
    key_allocation_succeeded = set->__key_allocator(key, key_space);
  } else {
    memcpy(key_space, key, set->__key_size);
    key_allocation_succeeded = 1;
  }

  if(!key_allocation_succeeded) {
    free(key_space);
    free(node);
    return NULL;
  }
  
  node->key = key_space;
  node->left = 0;
  node->right = 0;
  return node;
}

tree_set_s* tree_set_create_with_allocators(
  size_t key_size,
  tree_set_key_comparator_f key_comp,
  tree_set_key_allocator_f key_allocator,
  tree_set_key_deallocator_f key_deallocator
) {
  if(!key_size) {
    LOGE("treeset: error: key size cannot be zero\n");
    return NULL;
  }

  if(!key_comp) {
    LOGE("treeset: error: key comparator cannot be null\n");
    return NULL;
  }

  tree_set_wrapper_s* tree_set =
    (tree_set_wrapper_s*) malloc(sizeof(tree_set_wrapper_s));

  if(!tree_set) {
    LOGE("treeset: error: failed to allocate memory for tree set\n");
    return NULL;
  }

  tree_set->set.size = 0;
  tree_set->__root = 0;
  tree_set->__key_size = key_size;
  tree_set->__key_comp = key_comp;
  tree_set->__key_allocator = key_allocator;
  tree_set->__key_deallocator = key_deallocator;
  return &tree_set->set;
}

tree_set_s* tree_set_create_of_string_key() {
  return tree_set_create_with_allocators(
    sizeof(char*),
    &tree_set_string_key_comparator,
    &tree_set_string_key_allocator,
    &tree_set_string_key_deallocator
  );
}

tree_set_s* tree_set_create_of_int_key() {
  return tree_set_create_with_allocators(
    sizeof(int32_t),
    &tree_set_int_key_comparator,
    NULL,
    NULL
  );
}

tree_set_s* tree_set_create(size_t key_size, tree_set_key_comparator_f key_comp) {
  return tree_set_create_with_allocators(key_size, key_comp, NULL, NULL);
}

static void __tree_set_node_free(tree_set_wrapper_s* set, tree_set_node_s* root) {
  if(!root)
    return;

  __tree_set_node_free(set, root->left);
  __tree_set_node_free(set, root->right);

  if(set->__key_deallocator) 
    set->__key_deallocator(root->key);

  free(root->key);
  free(root);
}

static void __tree_set_free(tree_set_wrapper_s* set) {
  if(!set)
    return;

  __tree_set_node_free(set, set->__root);
  free(set);
}

void tree_set_free(tree_set_s* set) {
  __tree_set_free((tree_set_wrapper_s*) set);
}

static int32_t __tree_set_insert_helper(
  tree_set_wrapper_s* set,
  tree_set_node_s* root,
  tree_set_node_s* new_node
) {
  int32_t comparison = set->__key_comp(new_node->key, root->key);
  if(comparison == 0) {
    return 1;
  } else if(comparison < 0) {
    if(root->left)
      return __tree_set_insert_helper(set, root->left, new_node);

    root->left = new_node;
    return root->left != NULL;
  } else {
    if(root->right)
      return __tree_set_insert_helper(set, root->right, new_node);

    root->right = new_node;
    return root->right != NULL;
  }
}

static int32_t __tree_set_insert(tree_set_wrapper_s* set, tree_set_node_s* new_node) {
  if(set->__root)
    return __tree_set_insert_helper(set, set->__root, new_node);

  set->__root = new_node;
  return set->__root != NULL;
}

int32_t tree_set_insert(tree_set_s* set, void* key) {
  if(!set) {
    LOGE("treeset: error: cannot insert into a null tree set\n");
    return 0;
  }

  if(!key) {
    LOGE("treeset: error: cannot insert null key into tree set\n");
    return 0;
  }

  if(tree_set_contains(set, key))
    return 1;

  tree_set_node_s* new_node = __tree_set_node_create(((tree_set_wrapper_s*) set), key);
  if(!new_node) {
    LOGE("treeset: error: failed to alloc mem for new node\n");
    return 0;
  }

  int32_t inserted = __tree_set_insert(((tree_set_wrapper_s*) set), new_node);
  if(inserted)
    set->size++;

  return inserted;
}

static int32_t __tree_set_contains_helper(
  tree_set_wrapper_s* set,
  tree_set_node_s* node,
  void* key
) {
  int32_t comparison = set->__key_comp(key, node->key);
  if(comparison == 0) {
    return 1;
  } else if(comparison < 0) {
    if(node->left)
      return __tree_set_contains_helper(set, node->left, key);

    return 0;
  } else {
    if(node->right)
      return __tree_set_contains_helper(set, node->right, key);

    return 0;
  }
}

static int32_t __tree_set_contains(tree_set_wrapper_s* set, void* key) {
  if(!set->__root)
    return 0;

  return __tree_set_contains_helper(set, set->__root, key);
}

int32_t tree_set_contains(tree_set_s* set, void* key) {
  if(!set) {
    LOGE("treeset: error: cannot search in a null tree set\n");
    return 0;
  }

  if(!key) {
    LOGE("treeset: error: cannot search for a null key in tree set\n");
    return 0;
  }

  return __tree_set_contains((tree_set_wrapper_s*) set, key);
}

void __tree_set_print_helper(
  tree_set_wrapper_s* set,
  tree_set_node_s* root,
  void (*key_printer)(void*),
  void (*val_printer)(void*)
) {
  if(!root)
    return;

  __tree_set_print_helper(set, root->left, key_printer, val_printer);

  printf("node key*:%p ", root->key);
  key_printer(root->key);
  printf("\n");

  __tree_set_print_helper(set, root->right, key_printer, val_printer);
}

void __tree_set_print(
  tree_set_wrapper_s* set,
  void (*key_printer)(void*),
  void (*val_printer)(void*)
) {
  return __tree_set_print_helper(set, set->__root, key_printer, val_printer);
}

void tree_set_print(
  tree_set_s* set,
  void (*key_printer)(void*),
  void (*val_printer)(void*)
) {
  return __tree_set_print((tree_set_wrapper_s*) set, key_printer, val_printer);
}


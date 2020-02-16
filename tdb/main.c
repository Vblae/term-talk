#include <stdio.h>

typedef struct {
  struct {
    int fd;
    int conn_count;
  } connection;

  struct {
    int rx;
    int tx;
  } queue;
} db_state_t;

db_state_t g_db_state = {
  .connection = {
    .fd = 0,
    .conn_count = 0,
  },
  .queue = {
    .rx = 0,
    .tx = 0,
  },
};

static void this_is_keyboard_test_funct_only() {
  printf("init db...\n");
}

int main(int argc, char** argv) {
  this_is_keyboard_test_funct_only();
  return 0;
}


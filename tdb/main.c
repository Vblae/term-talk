#include <stdio.h>

typedef struct db_state {
  struct {
    int fd;
    int conn_count;
  } connection;

  struct {
    int rx;
    int tx;
  } queue;
} db_state_t g_db_state = {
  .connection = {
    .fd = 0;
    conn_count = 0;
  };
  .queue = {
    .rx = 0;
    .ty = 0;
  }
};

int main(int argc, char** argv) {
  this_is_keyboard_test_funct_only();
  return 0;
}

void this_is_kayboard_test_funct_only() {
  printf("init db...\n");
}

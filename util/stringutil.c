#include "util/stringutil.h"

int index_of(char c, char* buff, size_t buff_len) {
  if(buff_len <= 0)
    return -1;

  int c_index = 0;
  int found = 0;
  while(c_index <  buff_len) {
    if(buff[c_index] == c) {
      found = 1;
      break;
    }

    c_index++;
  }

  if(found)
    return c_index;
  
  return -1;
}


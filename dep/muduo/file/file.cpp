/*
* Created by boil on 2023/11/2.
*/

#include "file.h"

RD_NAMESPACE_BEGIN

  size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *stream) {
    int fd = fileno(stream);
    if (fd == -1) {
      return 0;
    }
    return write(fd, ptr, size * n) / size;
  }


RD_NAMESPACE_END
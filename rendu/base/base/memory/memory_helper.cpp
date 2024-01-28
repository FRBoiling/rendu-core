/*
* Created by boil on 2023/11/13.
*/

#include "memory_helper.h"

RD_NAMESPACE_BEGIN

void MemZero(void *p, std::size_t n) {
  memset(p, 0, n);
}

RD_NAMESPACE_END

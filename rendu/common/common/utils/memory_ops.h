/*
* Created by boil on 2023/11/5.
*/

#ifndef RENDU_COMMON_MEMORY_OPS_H
#define RENDU_COMMON_MEMORY_OPS_H

#include "define.h"

COMMON_NAMESPACE_BEGIN

namespace MemoryOps {

  inline void memZero(void *p, size_t n) {
    memset(p, 0, n);
  }
};

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_MEMORY_OPS_H

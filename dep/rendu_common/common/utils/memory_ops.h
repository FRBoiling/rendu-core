/*
* Created by boil on 2023/11/5.
*/

#ifndef RENDU_MEMORY_OPS_H
#define RENDU_MEMORY_OPS_H

#include "common/common_define.h"

RD_NAMESPACE_BEGIN

namespace MemoryOps {

  inline void memZero(void *p, size_t n) {
    memset(p, 0, n);
  }
};

RD_NAMESPACE_END

#endif //RENDU_MEMORY_OPS_H

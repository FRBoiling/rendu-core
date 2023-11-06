/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_BASE_BASE_MEMORY_MEMORY_HELPER_H_
#define RENDU_BASE_BASE_MEMORY_MEMORY_HELPER_H_

#include "base_define.h"
#include "number/def_helper.h"
#include <algorithm>

RD_NAMESPACE_BEGIN

void MemZero(void *p, SIZE n) {
  memset(p, 0, n);
}

RD_NAMESPACE_END

#endif//RENDU_BASE_BASE_MEMORY_MEMORY_HELPER_H_

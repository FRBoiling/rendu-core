/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_BASE_MEMORY_MEMORY_HELPER_H_
#define RENDU_BASE_MEMORY_MEMORY_HELPER_H_

#include "bit_converter.h"
#include "memory_copy.h"

#include <algorithm>

RD_NAMESPACE_BEGIN

extern void MemZero(void *p, std::size_t n);

RD_NAMESPACE_END

#endif//RENDU_BASE_MEMORY_MEMORY_HELPER_H_

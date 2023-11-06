/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_BASE_MEMORY_COPY_H
#define RENDU_BASE_MEMORY_COPY_H


#include "number/int_helper.h"
#include <algorithm>

RD_NAMESPACE_BEGIN

template<typename T>
void Copy(const T *src, INT32 src_index, T *dest, INT32 dest_index, INT32 length) {
  // 检查索引和长度是否合法，如果不合法，抛出异常
  if (src_index + length > sizeof(src) || dest_index + length > sizeof(dest)) {
    throw std::out_of_range("Array index out of range");
  }
  // 使用 std::copy 算法复制数组的一部分
  std::memcpy(dest + dest_index, src + src_index, sizeof(T) * length);
}

template<typename T>
void Copy(const T *src, T *dest) {
  if (sizeof(src) < sizeof(dest)) {
    // 长度是否合法，如果不合法，抛出异常
    // 目标内存过小 TODO：BOIL
    return;
  }
  std::memcpy(dest, src, sizeof(T));
}

RD_NAMESPACE_END

#endif//RENDU_BASE_MEMORY_COPY_H

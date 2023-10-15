/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_ARRAY_H
#define RENDU_ARRAY_H

#include "define.h"
#include <array>
#include <algorithm>

RD_NAMESPACE_BEGIN

    namespace Array {

        template<typename T>
        void Copy(const T *src, int32 src_index, T *dest, int32 dest_index, int32 length) {
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

    }

RD_NAMESPACE_END

#endif //RENDU_ARRAY_H

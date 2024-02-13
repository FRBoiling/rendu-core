/*
* Created by boil on 2024/2/12.
*/

#ifndef RENDU_MEMORY_MEMORY_MEMORY_EXTENSIONS_H_
#define RENDU_MEMORY_MEMORY_MEMORY_EXTENSIONS_H_

#include "memory_define.h"
#include <span>

MEMORY_NAMESPACE_BEGIN

class MemoryExtensions {
public:
  template<typename T>
  static std::span<T> AsSpan(std::vector<T> &vec) {
    return std::span<T>(vec);
  }

  template<typename T>
  static std::span<T> AsSpan(T *data,int size) {
    return std::span<T>(data, size);
  }

  template<typename T>
  static std::span<T> AsSpan(std::basic_string<T> &str) {
    return std::span<T>(str.data(), str.size());
  }
};

MEMORY_NAMESPACE_END
#endif//RENDU_MEMORY_MEMORY_MEMORY_EXTENSIONS_H_

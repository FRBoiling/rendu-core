/*
* Created by boil on 2024/2/7.
*/

#ifndef RENDU_MEMORY_MEMORY_I_BUFFER_WRITER_H_
#define RENDU_MEMORY_MEMORY_I_BUFFER_WRITER_H_

#include "memory_define.h"
#include "memory.h"

MEMORY_NAMESPACE_BEGIN

template<typename T>
class IBufferWriter {
public:
  virtual ~IBufferWriter() = default;

  // 对应C#中的Advance
  virtual void Advance(size_t count) = 0;

  // 对应C#中的GetMemory
  virtual Memory<T>& GetMemory(size_t sizeHint = 0) = 0;

  // 对应C#中的GetSpan
  virtual std::span<T> GetSpan(size_t sizeHint = 0) = 0;
};

MEMORY_NAMESPACE_END



#endif//RENDU_MEMORY_MEMORY_I_BUFFER_WRITER_H_

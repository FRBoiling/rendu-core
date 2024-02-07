/*
* Created by boil on 2024/2/7.
*/

#ifndef RENDU_MEMORY_H_
#define RENDU_MEMORY_H_

#include "memory_define.h"

#include <memory>
#include <span>

MEMORY_NAMESPACE_BEGIN

template<typename T>
class Memory {
private:
  std::unique_ptr<T[]> _object; // 使用 unique_ptr 管理内存
  int _index;      // 内存块的起始索引
  int _length;     // 内存块长度

public:
  // 构造函数
  Memory(T* array, int length) : _object(new T[length]), _index(0), _length(length) {
    std::copy(array, array + length, _object.get());
  }

  // 内存切片
  Memory<T> Slice(int start) {
    return Slice(start, _length - start);
  }

  // 内存切片，指定长度
  Memory<T> Slice(int start, int length) {
    Memory<T> slice(nullptr, length); // 创建一个空的Memory对象
    slice._object.reset(new T[length]); // 分配新的内存
    std::copy(_object.get() + start, _object.get() + start + length, slice._object.get());
    slice._index = start;
    slice._length = length;
    return slice;
  }

  // 转化为 span
  std::span<T> Span() {
    return std::span<T>(_object.get(), _length);
  }

  // 尝试拷贝至指定内存
  bool TryCopyTo(Memory<T> destination) {
    if (destination._length < _length) return false;
    std::copy(_object.get(), _object.get() + _length, destination._object.get());
    return true;
  }

  // 拷贝至指定内存
  void CopyTo(Memory<T> destination) {
    std::copy(_object.get(), _object.get() + _length, destination._object.get());
  }

  // 获取引用内存的长度
  int Length() {
    return _length;
  }

  // 检查内存块是否为空
  bool IsEmpty() {
    return _length == 0;
  }

  // 获取内存块的句柄
  std::unique_ptr<T[]> Pin() {
    return std::unique_ptr<T[]>(_object.release());
  }

  // 将内存块内容转为数组
  std::unique_ptr<T[]> ToArray() {
    std::unique_ptr<T[]> array(new T[_length]);
    std::copy(_object.get(), _object.get() + _length, array.get());
    return array;
  }
};

MEMORY_NAMESPACE_END

#endif//RENDU_MEMORY_MEMORY_MEMORY_H_

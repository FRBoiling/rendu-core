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
class Memory : public EqualityComparable<Memory<T>>{
private:
  std::shared_ptr<T[]> _object;
  int _start;
  int _end;

public:
  Memory() = default;

  Memory(T *array, int length)
      : _object(std::make_shared<T[]>(length)), _start(0), _end(length) {
    std::copy_n(array, length, _object.get());
  }

  Memory(std::shared_ptr<T[]> data, int start, int end)
      : _object(data), _start(start), _end(end) {}

  Memory<T> &Slice(int start, int length) {
    start += _start;
    if (start > _end || start + length > _end || start < _start) {
      // Handle error
    }
    _start = start;
    _end = start + length;
    return *this;
  }

  std::span<T> Span() const {
    return std::span<T>(_object.get() + _start, _end - _start);
  }

  bool TryCopyTo(Memory<T> &destination) {
    if (destination.Length() < Length()) return false;
    std::copy_n(_object.get() + _start, Length(), destination._object.get() + destination._start);
    return true;
  }

  void CopyTo(Memory<T> &destination) {
    if (!TryCopyTo(destination)) {
      // handle error
    }
  }

  int Length() const {
    return _end - _start;
  }

  bool IsEmpty() const {
    return Length() == 0;
  }

  std::shared_ptr<T[]> Pin() const {
    return _object;
  }

  T *ToArray() const {
    return _object.get() + _start;
  }

public:
  bool TheSameAs(const Memory<T>& other) const {
    if(_start != other._start || _end != other._end) return false;

    return std::equal(_object.get()+_start, _object.get()+_end, other._object.get()+other._start);
  }

public:
  friend bool operator==(const Memory<T>& lhs, const Memory<T>& rhs) {
    if (lhs._start != rhs._start || lhs._end != rhs._end) return false;

    return std::equal(lhs._object.get() + lhs._start,
                      lhs._object.get() + lhs._end,
                      rhs._object.get() + rhs._start);
  }
};


MEMORY_NAMESPACE_END

#endif//RENDU_MEMORY_H_

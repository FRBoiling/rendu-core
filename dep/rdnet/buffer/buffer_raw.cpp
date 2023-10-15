/*
* Created by boil on 2023/10/26.
*/

#include "buffer_raw.h"
#include "a_buffer.h"
#include "buffer_like_string.h"

RD_NAMESPACE_BEGIN

  StatisticImp(BufferRaw)

  BufferRaw::Ptr BufferRaw::Create() {
    return Ptr(new BufferRaw);
  }

  BufferRaw::BufferRaw(size_t capacity) {
    if (capacity) {
      SetCapacity(capacity);
    }
  }

  BufferRaw::BufferRaw(const char *data, size_t size) {
    Assign(data, size);
  }


  void BufferRaw::SetCapacity(size_t capacity) {
    if (_data) {
      do {
        if (capacity > _capacity) {
          //请求的内存大于当前内存，那么重新分配
          break;
        }

        if (_capacity < 2 * 1024) {
          //2K以下，不重复开辟内存，直接复用
          return;
        }

        if (2 * capacity > _capacity) {
          //如果请求的内存大于当前内存的一半，那么也复用
          return;
        }
      } while (false);

      delete[] _data;
    }
    _data = new char[capacity];
    _capacity = capacity;
  }

  BufferRaw::~BufferRaw() {
    if (_data) {
      delete[] _data;
    }
  }

  void BufferRaw::SetSize(size_t size) {
    if (size > _capacity) {
      throw std::invalid_argument("Buffer::setSize out of range");
    }
    _size = size;
  }

  void BufferRaw::Assign(const char *data, size_t size) {
    if (size <= 0) {
      size = strlen(data);
    }
    SetCapacity(size + 1);
    memcpy(_data, data, size);
    _data[size] = '\0';
    SetSize(size);
  }




RD_NAMESPACE_END
/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_BUFFER_OFFSET_H
#define RENDU_BUFFER_OFFSET_H

#include "a_buffer.h"
#include "utils/type_util.h"

RD_NAMESPACE_BEGIN

  template<typename D>
  class BufferOffset : public ABuffer {
  public:
    using Ptr = std::shared_ptr<BufferOffset>;

    explicit BufferOffset(D data, size_t offset = 0, size_t len = 0) : _data(std::move(data)) {
      setup(offset, len);
    }

    ~BufferOffset() override = default;

  public:
    char *Data() const override {
      return const_cast<char *>(GetPointer<D>(_data)->data()) + _offset;
    }

    size_t Size() const override {
      return _size;
    }

    std::string ToString() const override {
      return std::string(Data(), Size());
    }

  private:
    void setup(size_t offset = 0, size_t size = 0) {
      auto max_size = GetPointer<D>(_data)->size();
      assert(offset + size <= max_size);
      if (!size) {
        size = max_size - offset;
      }
      _size = size;
      _offset = offset;
    }

    template<typename T>
    static typename std::enable_if<is_pointer<T>::value, const T &>::type
    GetPointer(const T &data) {
      return data;
    }

    template<typename T>
    static typename std::enable_if<!is_pointer<T>::value, const T *>::type
    GetPointer(const T &data) {
      return &data;
    }

  private:
    D _data;
    size_t _size{};
    size_t _offset{};
  };

RD_NAMESPACE_END

#endif //RENDU_BUFFER_OFFSET_H

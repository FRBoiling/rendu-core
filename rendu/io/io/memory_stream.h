/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_IO_IO_MEMORY_STREAM_H_
#define RENDU_IO_IO_MEMORY_STREAM_H_

#include "io_define.h"
#include "stream.h"
#include <memory>
#include <sstream>
#include <stdexcept>

IO_NAMESPACE_BEGIN


class MemoryStream : public Stream,
                     public NonCopyable {
private:
  std::vector<std::byte> _buffer;
  size_t _origin;
  size_t _position;
  size_t _length;
  size_t _capacity;
  bool _expandable;
  bool _writable;
  bool _exposable;
  bool _isOpen;

public:
  MemoryStream();

  MemoryStream(int capacity);

  MemoryStream(std::span<std::byte> buffer, size_t index, size_t count, bool writable = true, bool publiclyVisible = false);

  ~MemoryStream();

public:
  MemoryStream &operator=(MemoryStream &&other) noexcept {
    if (this != &other) {
      _buffer = std::move(other._buffer);
      _origin = other._origin;
      _position = other._position;
      _length = other._length;
      _capacity = other._capacity;
      _expandable = other._expandable;
      _writable = other._writable;
      _exposable = other._exposable;
      _isOpen = other._isOpen;

      // 将other置于有效的空状态
      other._position = 0;
      other._length = 0;
      other._capacity = 0;
      other._isOpen = false;
    }
    return *this;
  }

public:
  bool CanRead() const override;
  bool CanWrite() const override;
  bool CanSeek() const override;

  size_t GetCapacity() const { return _capacity; };

  size_t GetLength() const override;
  void SetLength(size_t length) override;

  size_t GetPosition() const override;
  void SetPosition(size_t position) override;

  void Seek(size_t offset, SeekOrigin origin) override;

  void Write(std::span<std::byte> buffer, size_t offset, size_t count) override;
  size_t Read(std::span<std::byte> &buffer, size_t offset, size_t count) override;

  void WriteByte(std::byte b) override;
  int ReadByte() override;

  bool IsEndOfStream() const override;

  void CopyTo(Stream &destination) override;

  Task<void> WriteAsync(std::span<std::byte> buffer, size_t offset, size_t count) override;
  Task<size_t> ReadAsync(std::span<std::byte> &buffer, size_t offset, size_t count) override;
};


IO_NAMESPACE_END

#endif//RENDU_IO_IO_MEMORY_STREAM_H_

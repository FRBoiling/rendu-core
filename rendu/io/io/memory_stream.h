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

public:
  MemoryStream();

  MemoryStream(int capacity);

  MemoryStream(std::span<byte> buffer, int index, int count, bool writable = true, bool publiclyVisible = false);

  ~MemoryStream();

public:
  MemoryStream &operator=(MemoryStream &&other) noexcept {
    if (this != &other) {
      _buffer = std::move(other._buffer);
      _origin = other._origin;
      SetPosition(other.GetPosition());
      SetLength(other.GetLength());
      _capacity = other._capacity;
      _expandable = other._expandable;
      _writable = other._writable;
      _exposable = other._exposable;
      _isOpen = other._isOpen;

      // 将other置于有效的空状态
      other.SetPosition(0);
      other.SetLength(0);
      other._capacity = 0;
      other._isOpen = false;
    }
    return *this;
  }


public:
  bool CanRead() const override;
  bool CanWrite() const override;
  bool CanSeek() const override;

  int GetCapacity() const { return _capacity; };

  void SetLength(int length) override;
  void SetPosition(int position) override;

  void Seek(int offset, SeekOrigin origin) override;

  void Write(const byte *buffer, int offset, int count) override;
  void Write(const std::vector<byte> &buffer, int offset, int count) override;
  void Write(const std::span<byte> buffer) override;

  int Read(byte *buffer, int offset, int count) override;
  int Read(std::vector<byte> &buffer, int offset, int count) override;
  int Read(std::span<byte> buffer) override;

  bool IsEndOfStream() const;

  void CopyTo(Stream &destination, int buff_size) override;

  //  Task<void> WriteAsync(std::span<byte> buffer, int offset, int count) override;
  //  Task<int> ReadAsync(std::span<byte> &buffer, int offset, int count) override;

  std::vector<byte> &GetBuffer();
  bool TryGetBuffer(std::vector<byte> *&buffer);

private:
  bool EnsureCapacity(int required_capacity);

private:
  std::vector<byte> _buffer;
  int _origin;
  int _capacity;
  bool _expandable;
  bool _writable;
  bool _exposable;
  bool _isOpen;
};


IO_NAMESPACE_END

#endif//RENDU_IO_IO_MEMORY_STREAM_H_

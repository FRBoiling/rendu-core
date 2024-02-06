/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_IO_IO_STREAM_H_
#define RENDU_IO_IO_STREAM_H_

#include "io_define.h"
#include <span>

IO_NAMESPACE_BEGIN

enum class SeekOrigin {
  /// <summary>Specifies the beginning of a stream.</summary>
  Begin,
  /// <summary>Specifies the current position within a stream.</summary>
  Current,
  /// <summary>Specifies the end of a stream.</summary>
  End,
};

class Stream {
public:
public:
  virtual bool CanRead() const = 0;
  virtual bool CanWrite() const = 0;
  virtual bool CanSeek() const = 0;

  virtual size_t GetLength() const = 0;
  virtual void SetLength(size_t length) = 0;

  virtual size_t GetPosition() const = 0;
  virtual void SetPosition(size_t position) = 0;

  virtual void Seek(size_t offset, SeekOrigin origin) = 0;

  virtual void Write(std::span<std::byte> buffer, size_t offset, size_t count) = 0;
  virtual size_t Read(std::span<std::byte> &buffer, size_t offset, size_t count) = 0;

  virtual void WriteByte(std::byte b) = 0;
  virtual int ReadByte() = 0;

  virtual bool IsEndOfStream() const = 0;

  virtual void CopyTo(Stream &destination) = 0;

  virtual Task<void> WriteAsync(std::span<std::byte> buffer, size_t offset, size_t count) = 0;
  virtual Task<size_t> ReadAsync(std::span<std::byte> &buffer, size_t offset, size_t count) = 0;

  virtual void SetTimeout(int timeout){};
  virtual int GetTimeout() const { return 0; };
  virtual void Flush(){};
};

IO_NAMESPACE_END

#endif//RENDU_IO_IO_STREAM_H_

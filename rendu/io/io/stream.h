/*
* Created by boil on 2024/2/4.
*/

#ifndef RENDU_IO_IO_STREAM_H_
#define RENDU_IO_IO_STREAM_H_

#include "io_define.h"
#include <memory>
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
  virtual bool CanRead() const = 0;
  virtual bool CanWrite() const = 0;
  virtual bool CanSeek() const = 0;

  virtual void Write(const std::vector<byte> &buffer, int offset, int count) = 0;
  virtual void Write(const std::span<byte> buffer);

  virtual int Read(std::vector<byte> &buffer, int offset, int count) = 0;
  virtual int Read(std::span<byte> buffer);

  virtual int GetLength() const { return m_length; };
  virtual void SetLength(int length) { m_length = length; };
  virtual int GetPosition() const { return m_position; };
  virtual void SetPosition(int position) { m_position = position; };

  void CopyTo(Stream &destination);
  virtual void CopyTo(Stream &destination, int buff_size);


  //  virtual Task<void> WriteAsync(std::span<byte> buffer, int offset, int count) = 0;
  //  virtual Task<int> ReadAsync(std::span<byte> &buffer, int offset, int count) = 0;

  virtual void WriteByte(byte b);

  virtual int ReadByte();

  virtual void Seek(int offset, SeekOrigin origin){};

  virtual int GetTimeout() const { return 0; };
  virtual void Flush(){};

  virtual void SetTimeout(int timeout){};

  virtual void Close(){};

private:
  const int DefaultCopyBufferSize = 81920;
  int GetCopyBufferSize();


protected:
  Long m_position;
  Long m_length;
};

IO_NAMESPACE_END

#endif//RENDU_IO_IO_STREAM_H_

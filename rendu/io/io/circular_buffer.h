/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_IO_CIRCULAR_BUFFER_H
#define RENDU_IO_CIRCULAR_BUFFER_H

#include "io_define.h"
#include "stream.h"

IO_NAMESPACE_BEGIN

class CircularBuffer : public Stream {
public:
  CircularBuffer();
  ~CircularBuffer();


public:
  void SetFirstIndex(int index);
  int GetFirstIndex() const;

  void SetLastIndex(Long index);
  Long GetLastIndex() const;

  std::vector<byte> &GetFirst();

  std::vector<byte> &GetLast();

  void Read(Stream *stream, int count);

  void RemoveFirst();

  void AddLast();


  bool CanRead() const override;
  bool CanWrite() const override;
  bool CanSeek() const override;

  int GetLength() const override;

  void Write(const std::vector<byte> &buffer, int offset, int count) override;

  int Read(std::vector<byte> &buffer, int offset, int count) override;

  void Write(const std::span<byte> buffer) override;
  int Read(std::span<byte> buffer) override;

public:
  const short ChunkSize = 8192;

private:
  int m_first_index;
  Long m_last_index;

  std::queue<std::vector<byte> *> m_bufferQueue;
  std::queue<std::vector<byte> *> m_bufferCache;
  std::vector<byte> *m_lastBuffer;
};

IO_NAMESPACE_END

#endif//RENDU_IO_CIRCULAR_BUFFER_H

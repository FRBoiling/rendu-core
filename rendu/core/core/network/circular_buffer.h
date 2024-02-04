/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_CIRCULAR_BUFFER_H
#define RENDU_CIRCULAR_BUFFER_H

#include "core_define.h"
#include <vector>

CORE_NAMESPACE_BEGIN

class CircularBuffer : public Stream {
public:
  explicit CircularBuffer(std::size_t capacity)
      : m_first_index(0), m_last_index(0), m_size(0), m_capacity(capacity), m_head(0),
        m_tail(0) {}

public:
  void SetFirstIndex(int index);
  [[nodiscard]] INT32 GetFirstIndex() const;

  void SetLastIndex(int index);
  [[nodiscard]] INT64 GetLastIndex() const;

  INT64 GetLength();

  BYTE *GetFirst();

  BYTE *GetLast();

  INT32 Read(BYTE *buffer, INT32 offset, INT32 count);

  void Read(Stream *stream, int count);

  void Write(BYTE *buffer, INT32 offset, INT32 count);

  void RemoveFirst();

  void AddLast();

public:
  const INT16 ChunkSize = 8192;

private:
  INT32 m_first_index;
  INT64 m_last_index;

  Queue<BYTE *> m_bufferQueue;
  Queue<BYTE *> m_bufferCache;
  BYTE *m_lastBuffer;

  std::size_t m_size;    //大小
  std::size_t m_capacity;//容积
  std::size_t m_head;    //头
  std::size_t m_tail;    //尾
};

CORE_NAMESPACE_END

#endif//RENDU_CIRCULAR_BUFFER_H

/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_CIRCULAR_BUFFER_H
#define RENDU_CIRCULAR_BUFFER_H

#include "common/common_define.h"
#include "io/stream.h"
#include "queue.h"
#include <vector>

RD_NAMESPACE_BEGIN

    class CircularBuffer {
    public:
      explicit CircularBuffer(std::size_t capacity)
          : m_first_index(0), m_last_index(0), m_size(0), m_capacity(capacity), m_head(0),
            m_tail(0) {}

public:
      void SetFirstIndex(int index);
      [[nodiscard]] int32 GetFirstIndex() const;

      void SetLastIndex(int index);
      [[nodiscard]] int64 GetLastIndex() const;

      int64 GetLength();

      byte *GetFirst();

      byte *GetLast();

      int32 Read(byte *buffer, int32 offset, int32 count);

      void Read(Stream *stream, int count);

      void Write(byte *buffer, int32 offset, int32 count);

      void RemoveFirst();

      void AddLast();
    public:
      const int16 ChunkSize = 8192;
    private:
      int32 m_first_index;
      int64 m_last_index;

      Queue<byte *> m_bufferQueue;
      Queue<byte *> m_bufferCache;
      byte *m_lastBuffer;

      std::size_t m_size;       //大小
      std::size_t m_capacity;   //容积
      std::size_t m_head;       //头
      std::size_t m_tail;       //尾

    };

RD_NAMESPACE_END

#endif //RENDU_CIRCULAR_BUFFER_H

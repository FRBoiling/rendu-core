/*
* Created by boil on 2023/10/15.
*/

#include "circular_buffer.h"
#include "exception/exception.h"
#include "utils/array_helper.h"
#include "io/stream.h"

RD_NAMESPACE_BEGIN

    int32 CircularBuffer::Read(byte *buffer, int32 offset, int32 count) {
      if (sizeof(buffer) < offset + count) {
        throw Exception("bufferList length < count, buffer length: {} {} {}", sizeof(buffer), offset, count);
      }

      int64 length = GetLength();
      if (length < count) {
        count = (int) length;
      }

      int alreadyCopyCount = 0;
      while (alreadyCopyCount < count) {
        int n = count - alreadyCopyCount;
        if (ChunkSize - m_first_index > n) {
          Array::Copy(GetFirst(), m_first_index, buffer, alreadyCopyCount + offset, n);
          m_first_index += n;
          alreadyCopyCount += n;
        } else {
          Array::Copy(GetFirst(), m_first_index, buffer, alreadyCopyCount + offset, ChunkSize - m_first_index);
          alreadyCopyCount += ChunkSize - m_first_index;
          m_first_index = 0;
          RemoveFirst();
        }
      }

      return count;
    }

    void CircularBuffer::Read(Stream *stream, int count) {
      if (count > GetLength()) {
        throw Exception("bufferList length < count, {} {}", GetLength(), count);
      }

      int alreadyCopyCount = 0;
      while (alreadyCopyCount < count) {
        int n = count - alreadyCopyCount;
        if (ChunkSize - m_first_index > n) {
          stream->Write(GetFirst(), m_first_index, n);
          m_first_index += n;
          alreadyCopyCount += n;
        } else {
          stream->Write(GetFirst(), m_first_index, ChunkSize - m_first_index);
          alreadyCopyCount += ChunkSize - m_first_index;
          m_first_index = 0;
          RemoveFirst();
        }
      }
    }

    void CircularBuffer::RemoveFirst() {
      m_bufferCache.Enqueue(m_bufferQueue.Dequeue());
    }

    int64 CircularBuffer::GetLength() {
      return 0;
    }

    byte *CircularBuffer::GetFirst() {
      return nullptr;
    }

    byte *CircularBuffer::GetLast() {
      if (m_bufferQueue.Count() == 0) {
        AddLast();
      }
      return m_lastBuffer;
    }

    void CircularBuffer::AddLast() {
      byte *buffer;
      if (m_bufferCache.Count() > 0) {
        buffer = m_bufferCache.Dequeue();
      } else {
        buffer = new byte[ChunkSize];
      }
      m_bufferQueue.Enqueue(buffer);
      m_lastBuffer = buffer;
    }


    void CircularBuffer::Write(byte *buffer, int32 offset, int32 count) {
      int32 alreadyCopyCount = 0;
      while (alreadyCopyCount < count) {
        if (m_last_index == ChunkSize) {
          AddLast();
          m_last_index = 0;
        }

        int n = count - alreadyCopyCount;
        if (ChunkSize - m_last_index > n) {
          Array::Copy(buffer, alreadyCopyCount + offset, m_lastBuffer, m_last_index, n);
          m_last_index += count - alreadyCopyCount;
          alreadyCopyCount += n;
        } else {
          Array::Copy(buffer, alreadyCopyCount + offset, m_lastBuffer, m_last_index, ChunkSize - m_last_index);
          alreadyCopyCount += ChunkSize - m_last_index;
          m_last_index = ChunkSize;
        }
      }
    }

    int64 CircularBuffer::GetLastIndex() const {
      return m_last_index;
    }

    int32 CircularBuffer::GetFirstIndex() const {
      return m_first_index;
    }

    void CircularBuffer::SetLastIndex(int index) {
      m_last_index = index;
    }

    void CircularBuffer::SetFirstIndex(int index) {
      m_first_index = index;
    }

RD_NAMESPACE_END
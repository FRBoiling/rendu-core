/*
* Created by boil on 2023/10/15.
*/

#include "a_service.h"

CORE_NAMESPACE_BEGIN

    MemoryBuffer *AService::Fetch(int size) {
      if (size > MaxMemoryBufferSize) {
        return new MemoryBuffer(size);
      }

      if (size < MaxMemoryBufferSize) {
        size = MaxMemoryBufferSize;
      }

      if (m_pool.Count() == 0) {
        return new MemoryBuffer(size);
      }
      return m_pool.Dequeue();
    }

    void AService::Recycle(MemoryBuffer *&memoryBuffer) {
      if (memoryBuffer->capacity() > 1024)
      {
        return;
      }

      if (m_pool.Count() > 10) // 这里不需要太大，其实Kcp跟Tcp,这里1就足够了
      {
        return;
      }

      memoryBuffer->Seek(0, SeekOrigin::Begin);
      memoryBuffer->SetLength(0);

      m_pool.Enqueue(memoryBuffer);
    }

    std::tuple<uint32_t, uint32_t> AService::GetChannelConn(INT64 channelId) {
      throw Exception("default conn throw Exception! {}", channelId);
    }

    void AService::ChangeAddress(INT64 channelId, IPEndPoint ipEndPoint) {
    }

CORE_NAMESPACE_END
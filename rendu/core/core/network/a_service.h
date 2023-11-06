/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_A_SERVICE_H
#define RENDU_A_SERVICE_H

#include "common/define.h"
#include <functional>

#include  "network/ip_end_point.h"
#include "serialize/memory_buffer.h"
#include "queue.h"
#include "exception/exception_helper.h"
#include <tuple>

RD_NAMESPACE_BEGIN

    enum ServiceType {
      Outer,
      Inner,
    };

    class AService {

    protected:
      using AcceptCallback = std::function<void(int64, IPEndPoint *)>;
      using ReadCallback = std::function<void(int64, MemoryBuffer *)>;
      using ErrorCallback = std::function<void(int64, int32)>;

    public:
      int m_id;
      ServiceType m_serviceType;
      Queue<MemoryBuffer *> m_pool;
      const int MaxMemoryBufferSize = 1024;
    public:
      AcceptCallback OnAcceptCallback;
      ReadCallback OnReadCallback;
      ErrorCallback OnErrorCallback;

    public:
      MemoryBuffer *Fetch(int size = 0);

      void Recycle(MemoryBuffer *&memoryBuffer);

    public:
      [[nodiscard]] ServiceType GetServiceType() const;
      void SetServiceType(ServiceType type);

    public:
      virtual void Dispose() {};

      virtual void Update() = 0;

      virtual void Remove(int64 id, int error) = 0;

      virtual bool IsDisposed() = 0;

      virtual void Create(int64 id, std::string address) = 0;

      virtual void Send(int64 channelId, MemoryBuffer *memoryBuffer) = 0;

      [[maybe_unused]] virtual std::tuple<uint32_t, uint32_t> GetChannelConn(int64 channelId);

      virtual void ChangeAddress(int64 channelId, IPEndPoint ipEndPoint);


    };

    inline ServiceType AService::GetServiceType() const {
      return m_serviceType;
    }

    inline void AService::SetServiceType(ServiceType type) {
      m_serviceType = type;
    }

RD_NAMESPACE_END

#endif //RENDU_A_SERVICE_H

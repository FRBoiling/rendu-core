/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_A_SERVICE_H
#define RENDU_A_SERVICE_H

#include "core_define.h"

#include <functional>
#include <tuple>

#include "serialize/memory_buffer.h"

CORE_NAMESPACE_BEGIN

enum ServiceType {
  Outer,
  Inner,
};

class AService {

protected:
  using AcceptCallback = std::function<void(INT64, IPEndPoint *)>;
  using ReadCallback = std::function<void(INT64, MemoryBuffer *)>;
  using ErrorCallback = std::function<void(INT64, int32_t)>;

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
  virtual void Dispose(){};

  virtual void Update() = 0;

  virtual void Remove(INT64 id, int error) = 0;

  virtual bool IsDisposed() = 0;

  virtual void Create(INT64 id, IPEndPoint *ip_end_point) = 0;

  virtual void Send(INT64 channelId, MemoryBuffer *memoryBuffer) = 0;

  [[maybe_unused]] virtual std::tuple<uint32_t, uint32_t> GetChannelConn(INT64 channelId);

  virtual void ChangeAddress(INT64 channelId, IPEndPoint ipEndPoint);
};

inline ServiceType AService::GetServiceType() const {
  return m_serviceType;
}

inline void AService::SetServiceType(ServiceType type) {
  m_serviceType = type;
}

CORE_NAMESPACE_END

#endif//RENDU_A_SERVICE_H

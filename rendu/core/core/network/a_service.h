/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_A_SERVICE_H
#define RENDU_A_SERVICE_H

#include "core_define.h"

#include <functional>
#include <tuple>

CORE_NAMESPACE_BEGIN

enum ServiceType {
  Outer,
  Inner,
};

class AService {


protected:
  using AcceptCallback = std::function<void(Long, IPEndPoint *)>;
  using ReadCallback = std::function<void(Long, MemoryBuffer *)>;
  using ErrorCallback = std::function<void(Long, int32_t)>;

  int m_id;
  ServiceType m_serviceType;
  Queue<MemoryBuffer *> m_pool;
  
  const int MaxMemoryBufferSize = 1024;

public:
  AcceptCallback OnAcceptCallback;
  ReadCallback OnReadCallback;
  ErrorCallback OnErrorCallback;

public:
  inline ServiceType GetServiceType() const {
    return m_serviceType;
  }

  inline void SetServiceType(ServiceType type) {
    m_serviceType = type;
  }

  MemoryBuffer *Fetch(int size);

  void Recycle(MemoryBuffer *&memoryBuffer);

  virtual std::tuple<uint32_t, uint32_t> GetChannelConn(Long channelId);

  virtual void ChangeAddress(Long channelId, IPEndPoint ipEndPoint);

public:
  virtual void Dispose() = 0;

  virtual void Update() = 0;

  virtual void Remove(Long id, int error) = 0;

  virtual bool IsDisposed() = 0;

  virtual void Create(Long id, IPEndPoint *ip_end_point) = 0;

  virtual void Send(Long channelId, MemoryBuffer *memoryBuffer) = 0;
};


CORE_NAMESPACE_END

#endif//RENDU_A_SERVICE_H

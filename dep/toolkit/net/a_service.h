/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_A_SERVICE_H
#define RENDU_A_SERVICE_H

#include "event_loop.h"

RD_NAMESPACE_BEGIN

// server 基类, 暂时仅用于剥离 SessionHelper 对 TcpServer 的依赖
// 后续将 TCP 与 UDP 服务通用部分加到这里.
  class AService : public std::enable_shared_from_this<AService> {
  public:
    using Ptr = std::shared_ptr<AService>;

    explicit AService(EventLoop::Ptr poller = nullptr);

    virtual ~AService() = default;

  protected:
    EventLoop::Ptr _poller;
  };

RD_NAMESPACE_END

#endif //RENDU_A_SERVICE_H

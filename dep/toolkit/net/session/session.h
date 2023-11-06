/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_SESSION_H
#define RENDU_SESSION_H

#include "socket/socket_wrapper.h"

RD_NAMESPACE_BEGIN

  class AService;
  class TcpSession;
  class UdpSession;


  class Session : public SocketWrapper {
  public:
    using Ptr = std::shared_ptr<Session>;

    Session(const Socket::Ptr &sock);

    ~Session() override  = default;

    /**
     * 在创建 Session 后, Server 会把自身的配置参数通过该函数传递给 Session
     * @param server, 服务器对象
     */
    virtual void attachServer(const AService &server) {}

    /**
     * 作为该 Session 的唯一标识符
     * @return 唯一标识符
     */
    std::string GetIdentifier() const override;

  private:
    mutable std::string _id;
      std::unique_ptr<ObjectStatistic<TcpSession> > _statistic_tcp;
      std::unique_ptr<ObjectStatistic<UdpSession> > _statistic_udp;
  };



RD_NAMESPACE_END
#endif //RENDU_SESSION_H

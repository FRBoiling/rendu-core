/*
* Created by boil on 2023/10/26.
*/

#include "session.h"
#include "a_service.h"

RD_NAMESPACE_BEGIN

  class TcpSession : public Session {
  };

  class UdpSession : public Session {
  };

  StatisticImp(UdpSession)
  StatisticImp(TcpSession)

  Session::Session(const Socket::Ptr &sock) : SocketWrapper(sock) {
    if (sock->sockType() == SocketNum::Sock_TCP) {
      _statistic_tcp.reset(new ObjectStatistic<TcpSession>);
    } else {
      _statistic_udp.reset(new ObjectStatistic<UdpSession>);
    }
  }

  string Session::GetIdentifier() const {
    if (_id.empty()) {
      static std::atomic<uint64_t> s_session_index{0};
      _id = std::to_string(++s_session_index) + '-' + std::to_string(getSock()->rawFD());
    }
    return _id;
  }

RD_NAMESPACE_END
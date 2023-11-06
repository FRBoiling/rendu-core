/*
* Created by boil on 2023/10/22.
*/

#ifndef RENDU_SOCKET_H
#define RENDU_SOCKET_H

#include "define.h"
#include "i_socket.h"
#include "endpoint/ip_end_point.h"
#include "exception/socket_exception.h"
#include "socket_num.h"
#include "socket_fd.h"
#include "time/time_ticker.h"
#include "thread/mutex_wrapper.h"
#include "utils/bytes_speed.h"

RD_NAMESPACE_BEGIN


  class Socket : public std::enable_shared_from_this<Socket>, public ISocket {
  public:
    using Ptr = std::shared_ptr<Socket>;

  public:
    Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType);

    ~Socket() override;

  public:
    IPEndPoint *GetEndPoint() override;

    void Bind(IPEndPoint *endPoint) override;

    void Close() override;

    void Listen(int backlog) override;

    ISocket *Accept() override;

    void Connect(const char *remote_host, uint16_t remote_port) override;

  public:


    string GetLocalIp() override;

    uint16_t GetLocalPort() override;

    string GetRemoteIp() override;

    uint16_t GetRemotePort() override;

    string GetIdentifier() const override;

  public :
    IPEndPoint localEndPoint;
    IPEndPoint remoteEndPoint;


  };

RD_NAMESPACE_END

#endif //RENDU_SOCKET_H

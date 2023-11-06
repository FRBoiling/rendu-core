/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_ECHO_CLIENT_H
#define RENDU_ECHO_CLIENT_H

#include "network/tcp_client.h"
#include "network/tcp_connection.h"

#include "network/event_loop.h"
#include "logger/log.h"

RD_NAMESPACE_BEGIN



  class EchoClient : NonCopyable
  {
  public:
    EchoClient(EventLoop* loop, const IPEndPoint& listenAddr, const string& id);

    void connect();
    // void stop();

  private:
    void onConnection(const TcpConnectionPtr& conn);

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time);

    EventLoop* loop_;
    TcpClient client_;
  };

  extern std::vector<std::unique_ptr<EchoClient>> g_clients;
  extern int g_current;

RD_NAMESPACE_END

#endif //RENDU_ECHO_CLIENT_H

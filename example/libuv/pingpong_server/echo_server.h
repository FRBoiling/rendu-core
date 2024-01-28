/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_ECHO_SERVER_H
#define RENDU_ECHO_SERVER_H

#include "network/tcp_server.h"
#include "network/tcp_connection.h"

#include "network/event_loop.h"
#include "logger/log.h"

RD_NAMESPACE_BEGIN

class EchoServer
{
public:
  EchoServer(EventLoop* loop, const IPEndPoint& listenAddr);

  void start()
  {
    server_.Start();
  }
  // void stop();

private:
  void onConnection(const TcpConnectionPtr& conn);

  void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time);

private:
  EventLoop* loop_;
  TcpServer server_;
};

RD_NAMESPACE_END

#endif //RENDU_ECHO_SERVER_H

/*
* Created by boil on 2023/10/27.
*/

#include "echo_server.h"

RD_NAMESPACE_BEGIN

  EchoServer::EchoServer(EventLoop *loop, const IPEndPoint &listenAddr)
    : loop_(loop),
      server_(loop, listenAddr, "EchoServer") {
    server_.setConnectionCallback(
      std::bind(&EchoServer::onConnection, this, _1));
    server_.setMessageCallback(
      std::bind(&EchoServer::onMessage, this, _1, _2, _3));
    server_.setThreadNum(10);
  }

  void EchoServer::onConnection(const TcpConnectionPtr &conn) {
    LOG_TRACE << conn->peerAddress().ToString() << " -> "
              << conn->localAddress().ToString() << " is "
              << (conn->connected() ? "UP" : "DOWN");
    LOG_INFO << conn->getTcpInfoString();

    conn->send("hello\n");
  }

  void EchoServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {
    string msg(buf->retrieveAllAsString());
    LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
    if (msg == "exit\n")
    {
      conn->send("bye\n");
      conn->shutdown();
    }
    if (msg == "quit\n")
    {
      loop_->quit();
    }
    conn->send(msg);
  }

RD_NAMESPACE_END
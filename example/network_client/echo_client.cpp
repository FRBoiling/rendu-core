/*
* Created by boil on 2023/10/27.
*/

#include "echo_client.h"

RD_NAMESPACE_BEGIN

  int g_numThreads = 0;
  int g_current = 0;
  std::vector<std::unique_ptr<EchoClient>> g_clients;


  EchoClient::EchoClient(EventLoop *loop, const IPEndPoint &listenAddr, const string &id)
    : loop_(loop),
      client_(loop, listenAddr, "EchoClient" + id) {
    client_.setConnectionCallback(
      std::bind(&EchoClient::onConnection, this, _1));
    client_.setMessageCallback(
      std::bind(&EchoClient::onMessage, this, _1, _2, _3));
    //client_.enableRetry();
  }

  void EchoClient::connect() {
    client_.connect();
  }

  void EchoClient::onConnection(const TcpConnectionPtr &conn) {
    LOG_TRACE << conn->localAddress().ToString() << " -> "
              << conn->peerAddress().ToString() << " is "
              << (conn->connected() ? "UP" : "DOWN");

    if (conn->connected()) {
      ++g_current;
      if (TypeCast::implicit_cast<size_t>(g_current) < g_clients.size()) {
        g_clients[g_current]->connect();
      }
      LOG_INFO << "*** connected " << g_current;
    }
    conn->send("world\n");
  }

  void EchoClient::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {
    string msg(buf->retrieveAllAsString());
    LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
    if (msg == "quit\n") {
      conn->send("bye\n");
      conn->shutdown();
    } else if (msg == "shutdown\n") {
      loop_->quit();
    } else {
      conn->send(msg);
    }
  }

RD_NAMESPACE_END

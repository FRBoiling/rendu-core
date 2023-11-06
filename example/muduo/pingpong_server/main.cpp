//#include "echo_server.h"
#include "network/tcp_server.h"
#include "network/tcp_connection.h"

#include "network/event_loop.h"
#include "logger/log.h"

using namespace rendu;

//int main(int argc, char* argv[])
//{
////  LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
////  LOG_INFO << "sizeof TcpConnection = " << sizeof(TcpConnection);
//
//  EventLoop loop;
//  EchoServer server(&loop, IPEndPoint::FromString("127.0.0.1:11111").value());
//  server.start();
//
//  loop.loop();
//}


void onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    conn->setTcpNoDelay(true);
  }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
{
  conn->send(buf);
}

int main(int argc, char* argv[])
{
  // if (argc < 4)
  // {
  //   fprintf(stderr, "Usage: server <address> <port> <threads>\n");
  // }
  // else
  {
//    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
//    Logger::setLogLevel(Logger::WARN);

    // const char* ip = argv[1];
    // uint16_t port = static_cast<uint16_t>(atoi(argv[2]));

    const char* ip = "127.0.0.1";
    uint16_t port = 11111;
    IPEndPoint listenAddr = IPEndPoint::FromIpPort(ip, port).value();
    int threadCount = atoi(argv[3]);

    EventLoop loop;

    TcpServer server(&loop, listenAddr, "PingPong");

    server.SetConnectionCallback(onConnection);
    server.SetMessageCallback(onMessage);

    if (threadCount > 1)
    {
      server.SetThreadNum(threadCount);
    }

    server.Start();

    loop.Loop();
  }
}
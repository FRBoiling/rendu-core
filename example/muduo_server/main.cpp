#include <unistd.h>
#include "echo_server.h"


using namespace rendu;

int main(int argc, char* argv[])
{
//  LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
//  LOG_INFO << "sizeof TcpConnection = " << sizeof(TcpConnection);

  EventLoop loop;
  EchoServer server(&loop, IPEndPoint::FromString("127.0.0.1:11111").value());
  server.start();

  loop.loop();
}
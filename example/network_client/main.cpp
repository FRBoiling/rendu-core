#include "echo_client.h"

using namespace rendu;

int main(int argc, char *argv[]) {
//  LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  EventLoop loop;
  g_clients.reserve(1);
  for (int i = 0; i < 1; ++i) {
    char buf[32];
    snprintf(buf, sizeof buf, "%d", i + 1);
    g_clients.emplace_back(new EchoClient(&loop, IPEndPoint::FromString("127.0.0.1:11111").value(), buf));
  }

  g_clients[g_current]->connect();
  loop.loop();
}
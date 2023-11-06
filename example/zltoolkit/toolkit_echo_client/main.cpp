#include "tcp/tcp_client.h"
#include "echo_session.h"
#include "logger/log.h"

using namespace std;
using namespace rendu;


int main() {
  // 设置日志系统
  Logger::Instance().Add(std::make_shared<ConsoleChannel>());
  Logger::Instance().SetWriter(std::make_shared<AsyncLogWriter>());

  TcpClient::Ptr client(new TcpClient());//必须使用智能指针
  client->Connect<EchoSession>("192.168.1.101",9000);//连接服务器

  //退出程序事件处理
  static Semaphore sem;
  signal(SIGINT, [](int) { sem.Post(); });// 设置退出信号
  sem.Wait();
  return 0;
}
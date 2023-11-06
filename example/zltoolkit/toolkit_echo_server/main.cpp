#include "tcp/tcp_server.h"
#include "echo_session.h"
#include "logger/log.h"

using namespace rendu;

int main(int, char *[]) {

  //初始化日志模块
  Logger::Instance().Add(std::make_shared<ConsoleChannel>());
  Logger::Instance().SetWriter(std::make_shared<AsyncLogWriter>());
  Logger::Instance().SetLevel(LogLevel::LTrace);

  LOG_TRACE<< ">>>>LOG_TRACE<<<";
  LOG_DEBUG<< ">>>>LOG_DEBUG<<<";
  LOG_INFO<< ">>>>LOG_INFO<<<";
  LOG_WARN<< ">>>>LOG_WARN<<<";
  LOG_ERROR<< ">>>>LOG_ERROR<<<";
  LOG_CRITICAL<< ">>>>LOG_CRITICAL<<<";


  TcpServer::Ptr server(new TcpServer());
  server->Listen<EchoSession>(9000);//监听9000端口

  //退出程序事件处理
  static Semaphore sem;
  signal(SIGINT, [](int) { sem.Post(); });// 设置退出信号
  sem.Wait();


  return 0;
}
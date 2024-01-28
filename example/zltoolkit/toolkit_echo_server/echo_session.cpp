/*
* Created by boil on 2023/10/27.
*/

#include "echo_session.h"



rendu::EchoSession::EchoSession(const rendu::Socket::Ptr &sock) :
  Session(sock) {
  LOG_DEBUG;
}

rendu::EchoSession::~EchoSession() {
  LOG_DEBUG;
}

void rendu::EchoSession::onRecv(const rendu::ABuffer::Ptr &buf) {
  //处理客户端发送过来的数据
  LOG_TRACE << buf->Data() << " from GetPort:" << GetLocalPort();
//  Send(buf);
}

void rendu::EchoSession::onConnect(const rendu::SockException &ex) {
  //连接结果事件
  LOG_INFO << (ex ?  ex.what() : "success");
}

void rendu::EchoSession::onError(const rendu::SockException &ex) {
  //断开连接事件，一般是EOF
  LOG_WARN << ex.what();
}


void rendu::EchoSession::onFlush() {
  //发送阻塞后，缓存清空事件
  LOG_DEBUG;
}

void rendu::EchoSession::onManager() {
  //定时管理该对象，譬如会话超时检查
  LOG_DEBUG;
}

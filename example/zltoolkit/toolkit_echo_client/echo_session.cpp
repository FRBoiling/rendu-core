/*
* Created by boil on 2023/10/27.
*/

#include "echo_session.h"

rendu::EchoSession::EchoSession(const rendu::Socket::Ptr &sock) :
  Session(sock) {
  DebugL;
}

rendu::EchoSession::~EchoSession() {
  DebugL;
}

void rendu::EchoSession::onRecv(const rendu::ABuffer::Ptr &buf) {
  //处理客户端发送过来的数据
  TraceL << buf->Data() << " from GetPort:" << GetLocalPort();
//  Send(buf);
}

void rendu::EchoSession::onError(const rendu::SockException &err) {
  //客户端断开连接或其他原因导致该对象脱离TCPServer管理
  WarnL << err;
}

void rendu::EchoSession::onConnect(const rendu::SockException &ex) {

}

void rendu::EchoSession::onFlush() {

}

void rendu::EchoSession::onManager() {
  //定时发送数据到服务器
  auto buf = BufferRaw::Create();
  if(buf){
    buf->Assign("[BufferRaw]\0");
    (*this) << _nTick++ << " "
            << 3.14 << " "
            << string("string") << " "
            <<(ABuffer::Ptr &)buf;
  }
}

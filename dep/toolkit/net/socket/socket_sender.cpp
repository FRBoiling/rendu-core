/*
* Created by boil on 2023/10/26.
*/

#include "socket_sender.h"
#include "buffer/buffer_raw.h"
#include "buffer/buffer_string.h"

RD_NAMESPACE_BEGIN

  ssize_t SocketSender::send(string buf) {
    return Send(std::make_shared<BufferString>(std::move(buf)));
  }

  ssize_t SocketSender::send(const char *buf, size_t size) {
    auto buffer = BufferRaw::Create();
    buffer->Assign(buf, size);
    return Send(std::move(buffer));
  }

  SocketSender &SocketSender::operator<<(const char *buf) {
    send(buf);
    return *this;
  }

  SocketSender &SocketSender::operator<<(string buf) {
    send(std::move(buf));
    return *this;
  }

  SocketSender &SocketSender::operator<<(ABuffer::Ptr buf) {
    Send(std::move(buf));
    return *this;
  }


RD_NAMESPACE_END
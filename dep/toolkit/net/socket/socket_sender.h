/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_SOCKET_SENDER_H
#define RENDU_SOCKET_SENDER_H

#include "buffer/a_buffer.h"
#include "exception/socket_exception.h"
#include <sstream>

RD_NAMESPACE_BEGIN

  class SocketSender {
  public:
    SocketSender() = default;
    virtual ~SocketSender() = default;

    virtual ssize_t Send(ABuffer::Ptr buf) = 0;
    virtual void Shutdown(const SockException &ex = SockException(Err_shutdown, "self shutdown")) = 0;

    //发送char *
    SocketSender &operator << (const char *buf);
    //发送字符串
    SocketSender &operator << (std::string buf);
    //发送Buffer对象
    SocketSender &operator << (ABuffer::Ptr buf);

    //发送其他类型是数据
    template<typename T>
    SocketSender &operator << (T &&buf) {
      std::ostringstream ss;
      ss << std::forward<T>(buf);
      send(ss.str());
      return *this;
    }

    ssize_t send(std::string buf);
    ssize_t send(const char *buf, size_t size = 0);
  };




RD_NAMESPACE_END

#endif //RENDU_SOCKET_SENDER_H

/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_SOCKET_NUM_H
#define RENDU_SOCKET_NUM_H

#include "define.h"
#include "utils/socket_helper.h"

RD_NAMESPACE_BEGIN

class SocketNum {
public:
  using Ptr = std::shared_ptr<SocketNum>;

  typedef enum {
    Sock_Invalid = -1,
    Sock_TCP = 0,
    Sock_UDP = 1,
    Sock_TCP_Server = 2
  } SockType;

  SocketNum(int fd, SockType type);

  ~SocketNum() {
#if defined (OS_IPHONE)
    unsetSocketOfIOS(_fd);
#endif //OS_IPHONE
    // 停止socket收发能力
#if defined(_WIN32)
    ::shutdown(_fd, SD_BOTH);
#else
    shutdown(_fd, SHUT_RDWR);
#endif
    SocketHelper::Close(_fd);
  }

  int32 RawFd() const;

  [[maybe_unused]] SockType GetType();

  void SetConnected();

private:
  int32 _fd;
  SockType _type;
};

RD_NAMESPACE_END

#endif //RENDU_SOCKET_NUM_H

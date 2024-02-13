/*
* Created by boil on 2024/3/13.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_SHUTDOWN_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_SHUTDOWN_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

enum class SocketShutdown {
  // Shutdown sockets for receive.
  Receive = 0x00,

  // Shutdown socket for send.
  Send = 0x01,

  // Shutdown socket for both send and receive.
  Both = 0x02,
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_SHUTDOWN_H_

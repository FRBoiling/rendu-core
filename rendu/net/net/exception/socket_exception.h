/*
* Created by boil on 2024/2/12.
*/

#ifndef RENDU_NET_NET_EXCEPTION_SOCKET_EXCEPTION_H_
#define RENDU_NET_NET_EXCEPTION_SOCKET_EXCEPTION_H_

#include "net_define.h"
#include "sockets/socket_error.h"

NET_NAMESPACE_BEGIN

class SocketException : public std::exception {
public:
  SocketException(SocketError socket_error){}
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_EXCEPTION_SOCKET_EXCEPTION_H_

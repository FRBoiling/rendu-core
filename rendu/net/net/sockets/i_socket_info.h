/*
* Created by boil on 2024/3/11.
*/

#ifndef RENDU_NET_NET_SOCKETS_I_SOCKET_INFO_H_
#define RENDU_NET_NET_SOCKETS_I_SOCKET_INFO_H_

#include "net_define.h"
#include "endpoint/end_point.h"

NET_NAMESPACE_BEGIN

class ISocketInfo {
public:
  ISocketInfo() = default;
  virtual ~ISocketInfo() = default;

  //获取本机地址和端口号
  virtual EndPoint& GetLocalEndPoint() = 0;
  //获取远端地址和端口号
  virtual EndPoint& GetRemoteEndPoint() = 0;
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_I_SOCKET_INFO_H_

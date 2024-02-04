/*
* Created by boil on 2024/2/5.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_OPERATION_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_OPERATION_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

enum class SocketAsyncOperation {
  /// <summary><para>None of the socket operations.</para></summary>
  None,
  /// <summary><para>A socket Accept operation. </para></summary>
  Accept,
  /// <summary><para>A socket Connect operation.</para></summary>
  Connect,
  /// <summary><para>A socket Disconnect operation.</para></summary>
  Disconnect,
  /// <summary><para>A socket Receive operation.</para></summary>
  Receive,
  /// <summary><para>A socket ReceiveFrom operation.</para></summary>
  ReceiveFrom,
  /// <summary><para>A socket ReceiveMessageFrom operation.</para></summary>
  ReceiveMessageFrom,
  /// <summary><para>A socket Send operation.</para></summary>
  Send,
  /// <summary><para>A socket SendPackets operation.</para></summary>
  SendPackets,
  /// <summary><para>A socket SendTo operation.</para></summary>
  SendTo,
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_OPERATION_H_

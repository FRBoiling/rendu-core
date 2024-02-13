/*
* Created by boil on 2024/2/18.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_ERROR_PAL_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_ERROR_PAL_H_

#include "net_define.h"

#include "socket_error.h"
#include "interop/error_info.h"

NET_NAMESPACE_BEGIN

class SocketErrorPal {

private:
  static std::unordered_map<interop::Error, SocketError> s_native_to_socket_error;
  static std::unordered_map<SocketError, interop::Error> s_socket_to_native_error;

public:
  static std::optional<SocketError> GetSocketErrorForNativeError(interop::Error errno_code);
  static std::optional<interop::Error> GetNativeErrorForSocketError(SocketError socket_error);
};


NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_ERROR_PAL_H_

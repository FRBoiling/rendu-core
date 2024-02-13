/*
* Created by boil on 2024/2/22.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_EXCEPTION_FACTORY_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_EXCEPTION_FACTORY_H_

#include "socket_exception.h"

#include "endpoint/end_point.h"
#include "socket_error_pal.h"

NET_NAMESPACE_BEGIN

class SocketExceptionFactory {
public:
  static SocketException* CreateSocketException(int socketError, EndPoint &endPoint) {
    int nativeSocketError = socketError;
    if (auto native_err = SocketErrorPal::GetNativeErrorForSocketError((SocketError &) socketError)) {
      nativeSocketError = interop::ErrorInfo(*native_err).GetRawErrno();
    }
    return new SocketException((SocketError&)socketError, CreateMessage(nativeSocketError, endPoint));
  }

private:
  static string CreateMessage(int nativeSocketError, EndPoint &endPoint) {
    return interop::GetErrorMsg(nativeSocketError) + " " + endPoint.ToString();
  }
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_EXCEPTION_FACTORY_H_

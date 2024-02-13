/*
* Created by boil on 2024/2/18.
*/

#ifndef RENDU_NET_NET_SOCKETS_SAFE_SOCKET_HANDLE_H_
#define RENDU_NET_NET_SOCKETS_SAFE_SOCKET_HANDLE_H_

#include "interop/sys.h"
#include "net_define.h"
#include "socket_type.h"

NET_NAMESPACE_BEGIN

class SocketAsyncContext;

class SafeSocketHandle : public interop::SafeHandle {
public:
  using Ptr = std::shared_ptr<SafeSocketHandle>;

public:
  SafeSocketHandle();

  SafeSocketHandle(SocketType type);

  SafeSocketHandle(int pre_existing_fd, bool);

  ~SafeSocketHandle();

public:
  bool m_last_connect_failed;
  bool m_is_disconnected;
  std::unique_ptr<SocketAsyncContext> m_async_context;

public:
  int GetReceiveTimeout() const {
    return m_receive_timeout;
  }

  int SetReceiveTimeout(int timeout) {
    m_receive_timeout = timeout;
    return m_receive_timeout;
  }

  int GetSendTimeout() const {
    return m_send_timeout;
  }
  int SetSendTimeout(int timeout) {
    m_send_timeout = timeout;
    return m_send_timeout;
  }

  void RegisterConnectResult(SocketError error) {
    switch (error) {
      case SocketError::Success:
      case SocketError::WouldBlock:
        break;
      default:
        m_last_connect_failed = true;
        break;
    }
  }


  void TrackOption(SocketOptionLevel level, SocketOptionName name) {
    //    switch (level)
    //    {
    //      case SocketOptionLevel::IP:
    //        if (name != SocketOptionName::ReuseAddress)
    //        {
    //          if (name == SocketOptionName::DontFragment)
    //          {
    //            _trackedOptions |= TrackedSocketOptions::DontFragment;
    //            return;
    //          }
    //          break;
    //        }
    //        this._trackedOptions |= TrackedSocketOptions.Ttl;
    //        return;
    //      case SocketOptionLevel::Tcp:
    //        if (name == SocketOptionName.Debug)
    //        {
    //          this._trackedOptions |= TrackedSocketOptions.NoDelay;
    //          return;
    //        }
    //        break;
    //      case SocketOptionLevel.IPv6:
    //        if (name != SocketOptionName.ReuseAddress)
    //        {
    //          if (name == SocketOptionName.IPv6Only)
    //          {
    //            this._trackedOptions |= TrackedSocketOptions.DualMode;
    //            return;
    //          }
    //          break;
    //        }
    //        this._trackedOptions |= TrackedSocketOptions.Ttl;
    //        return;
    //      case SocketOptionLevel.Socket:
    //        switch (name)
    //        {
    //          case SocketOptionName.Broadcast:
    //            this._trackedOptions |= TrackedSocketOptions.EnableBroadcast;
    //            return;
    //          case SocketOptionName.Linger:
    //            this._trackedOptions |= TrackedSocketOptions.LingerState;
    //            return;
    //          case SocketOptionName.SendBuffer:
    //            this._trackedOptions |= TrackedSocketOptions.SendBufferSize;
    //            return;
    //          case SocketOptionName.ReceiveBuffer:
    //            this._trackedOptions |= TrackedSocketOptions.ReceiveBufferSize;
    //            return;
    //          case SocketOptionName.SendTimeout:
    //            this._trackedOptions |= TrackedSocketOptions.SendTimeout;
    //            return;
    //          case SocketOptionName.ReceiveTimeout:
    //            this._trackedOptions |= TrackedSocketOptions.ReceiveTimeout;
    //            return;
    //        }
    //        break;
    //    }
    //    this.ExposedHandleOrUntrackedConfiguration = true;
  }

public:
  interop::Error CloseSocket()  {
    interop::Error error = interop::Error::RD_SUCCESS;
    if (Close() != 0) {
      error = interop::Sys::GetLastError();
      if (error == interop::Error::RD_ECONNRESET) {
        error = interop::Error::RD_SUCCESS;
      }
    }
    return error;
  }

  bool HasShutdownSend() { return m_has_shutdown_send; }


private:
  SocketType m_type;
  int m_receive_timeout;
  int m_send_timeout;
  bool m_has_shutdown_send;
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SAFE_SOCKET_HANDLE_H_

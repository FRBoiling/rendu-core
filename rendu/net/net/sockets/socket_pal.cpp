/*
* Created by boil on 2024/2/14.
*/

#include "socket_pal.h"

#include "address/ip_address.h"
#include "interop/sys.h"
#include "socket_error_pal.h"

#include "ip_v6_multicast_option.h"
#include "multicast_option.h"
#include "poller/poll_events.h"

NET_NAMESPACE_BEGIN

using namespace interop;


SocketError SocketPal::GetSockName(SafeSocketHandle::Ptr socket, byte *addr, int &addr_len) {
  interop::Error err = interop::Sys::GetSockName(socket, addr, addr_len);
  return err != interop::Error::RD_SUCCESS ? SocketPal::GetSocketErrorForErrorCode(err) : SocketError::Success;
}

SocketError SocketPal::GetPeerName(SafeSocketHandle::Ptr socket, byte *addr, int &addr_len) {
  interop::Error err = interop::Sys::GetPeerName(socket, addr, addr_len);
  return err != interop::Error::RD_SUCCESS ? SocketPal::GetSocketErrorForErrorCode(err) : SocketError::Success;
}

SafeSocketHandle::Ptr SocketPal::CreateSocket(int fd) {
  return std::make_shared<SafeSocketHandle>(fd, true);
}

SocketError SocketPal::CreateSocket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType, SafeSocketHandle::Ptr& socket) {
  socket = std::make_shared<SafeSocketHandle>();
  int sockfd = 0;
  interop::Error errorCode1 = interop::Sys::Socket(static_cast<int>(addressFamily), static_cast<int>(socketType), static_cast<int>(protocolType), sockfd);

  SocketError result;
  if (errorCode1 == interop::Error::RD_SUCCESS) {
    result = SocketError::Success;
    if (addressFamily == AddressFamily::InterNetworkV6 && socketType != SocketType::Raw) {
      int optval = 1;
      interop::Error errorCode2 = interop::Sys::SetSockOpt(sockfd, SocketOptionLevel::IPv6, SocketOptionName::IPv6Only, (byte *) &optval, sizeof(optval));
      if (errorCode2 != interop::Error ::RD_SUCCESS) {
        interop::Sys::Close(sockfd);
        sockfd = -1;
        result = SocketPal::GetSocketErrorForErrorCode(errorCode2);
      }
    }
  } else {
    result = SocketPal::GetSocketErrorForErrorCode(errorCode1);
  }
  socket->SetHandle(sockfd);
  if (socket->IsInvalid())
    socket.reset();

  return result;
}

SocketError SocketPal::SetSockOpt(SafeSocketHandle::Ptr handle, SocketOptionLevel optionLevel, SocketOptionName optionName, std::span<byte> optionValue) {
  interop::Error err = interop::Sys::SetSockOpt(handle, optionLevel, optionName, optionValue.data(), optionValue.size());
  return GetErrorAndTrackSetting(handle, optionLevel, optionName, err);
}

SocketError SocketPal::SetSockOpt(SafeSocketHandle::Ptr handle, SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue) {
  return SetSockOpt(handle, optionLevel, optionName, std::span<byte>(reinterpret_cast<byte*>(&optionValue), sizeof(optionValue)));
}

SocketError SocketPal::GetSockOpt(SafeSocketHandle::Ptr handle, SocketOptionLevel optionLevel, SocketOptionName optionName, int &optionValue) {
  if (optionLevel == SocketOptionLevel::Socket) {
    if (optionName == SocketOptionName::ReceiveTimeout) {
      optionValue = handle->GetReceiveTimeout() == -1 ? 0 : handle->GetReceiveTimeout();
      return SocketError::Success;
    }
    if (optionName == SocketOptionName::SendTimeout) {
      optionValue = handle->GetSendTimeout() == -1 ? 0 : handle->GetSendTimeout();
      return SocketError::Success;
    }
  }
  if (optionName == SocketOptionName::Error) {
    interop::Error errorCode = interop::Error::RD_SUCCESS;
    interop::Error socketErrorOption = interop::Sys::GetSocketErrorOption(handle, errorCode);
    optionValue = (int) SocketPal::GetSocketErrorForErrorCode(errorCode);
    return socketErrorOption != interop::Error::RD_SUCCESS ? SocketPal::GetSocketErrorForErrorCode(socketErrorOption) : SocketError::Success;
  }
  int num1 = 0;
  int num2 = 4;
  interop::Error sockOpt = interop::Sys::GetSockOpt(handle, optionLevel, optionName, (byte *) &num1, &num2);
  optionValue = num1;
  return sockOpt != interop::Error::RD_SUCCESS ? SocketPal::GetSocketErrorForErrorCode(sockOpt) : SocketError::Success;
}


SocketError SocketPal::GetErrorAndTrackSetting(SafeSocketHandle::Ptr handle, SocketOptionLevel optionLevel, SocketOptionName optionName, interop::Error err) {
  if (err != interop::Error::RD_SUCCESS) {
    return GetSocketErrorForErrorCode(err);
  }
  handle->TrackOption(optionLevel, optionName);
  return SocketError::Success;
}


SocketError SocketPal::GetSocketErrorForErrorCode(interop::Error error_code) {
  if (auto socket_error = SocketErrorPal::GetSocketErrorForNativeError(error_code)) {
    return *socket_error;
  }
  return SocketError::Error;
}

SocketError SocketPal::GetLingerOption(SafeSocketHandle::Ptr handle, LingerOption *&optionValue) {
  interop::Sys::LingerOption *lingerOption1 = nullptr;
  interop::Error lingerOption2 = interop::Sys::GetLingerOption(handle, lingerOption1);
  if (lingerOption2 != interop::Error::RD_SUCCESS) {
    optionValue = nullptr;
    return SocketPal::GetSocketErrorForErrorCode(lingerOption2);
  }
  optionValue = new LingerOption(lingerOption1->m_on_off != 0, lingerOption1->m_seconds);
  return SocketError::Success;
}

SocketError SocketPal::GetMulticastOption(SafeSocketHandle::Ptr handle, SocketOptionName optionName, MulticastOption *&optionValue) {
  interop::Sys::MulticastOption multicastOption = optionName == SocketOptionName::AddMembership
                                                      ? interop::Sys::MulticastOption::MULTICAST_ADD
                                                      : interop::Sys::MulticastOption::MULTICAST_DROP;
  interop::Sys::IPv4MulticastOption *ipv4MulticastOption1 = nullptr;
  interop::Error ipv4MulticastOption2 = interop::Sys::GetIPv4MulticastOption(handle, multicastOption, ipv4MulticastOption1);
  if (ipv4MulticastOption2 != interop::Error::RD_SUCCESS) {
    optionValue = nullptr;
    return SocketPal::GetSocketErrorForErrorCode(ipv4MulticastOption2);
  }
  IPAddress group((long) ipv4MulticastOption1->MulticastAddress);
  IPAddress mcint((long) ipv4MulticastOption1->LocalAddress);
  optionValue = new MulticastOption(group, mcint, ipv4MulticastOption1->InterfaceIndex);
  return SocketError::Success;
}

SocketError SocketPal::GetIPv6MulticastOption(SafeSocketHandle::Ptr handle, SocketOptionName optionName, IPv6MulticastOption *&optionValue) {
  interop::Sys::MulticastOption multicastOption = optionName == SocketOptionName::AddMembership
                                                      ? interop::Sys::MulticastOption::MULTICAST_ADD
                                                      : interop::Sys::MulticastOption::MULTICAST_DROP;
  interop::Sys::IPv6MulticastOption *ipv6MulticastOption1 = nullptr;
  interop::Error ipv6MulticastOption2 = interop::Sys::GetIPv6MulticastOption(handle, multicastOption, ipv6MulticastOption1);
  if (ipv6MulticastOption2 != interop::Error::RD_SUCCESS) {
    optionValue = nullptr;
    return SocketPal::GetSocketErrorForErrorCode(ipv6MulticastOption2);
  }
  IPAddress ip_address(ipv6MulticastOption1->Address.Address);

  optionValue = new IPv6MulticastOption(ip_address, (Long) ipv6MulticastOption1->InterfaceIndex);
  return SocketError::Success;
}


SocketError SocketPal::Bind(SafeSocketHandle::Ptr handle, ProtocolType socketProtocolType, std::span<byte> buffer) {
  interop::Error errorCode = interop::Sys::Bind(handle, socketProtocolType, buffer);
  return errorCode != interop::Error::RD_SUCCESS ? SocketPal::GetSocketErrorForErrorCode(errorCode) : SocketError::Success;
}

SocketError SocketPal::Listen(SafeSocketHandle::Ptr handle, int backlog) {
  interop::Error errorCode = interop::Sys::Listen(handle, backlog);
  return errorCode != interop::Error::RD_SUCCESS ? SocketPal::GetSocketErrorForErrorCode(errorCode) : SocketError::Success;
}

SocketError SocketPal::Accept(SafeSocketHandle::Ptr listenSocket, Memory<byte> &&socketAddress, int &socketAddressLen, SafeSocketHandle::Ptr socket) {
  socket = std::make_shared<SafeSocketHandle>();

  int acceptedFd;
  SocketError errorCode;
  //  if (!listenSocket.IsNonBlocking) {
  //    errorCode = listenSocket.AsyncContext.Accept(socketAddress, out socketAddressLen, out acceptedFd);
  //  } else {
  if (!TryCompleteAccept(listenSocket, socketAddress, socketAddressLen, acceptedFd, errorCode)) {
    errorCode = SocketError::WouldBlock;
  }
  //  }
  return errorCode;
}

SocketError SocketPal::Connect(SafeSocketHandle::Ptr socket, Memory<byte> socketAddress) {
  //  if (!socket.IsNonBlocking) {
  //    return socket.AsyncContext.Connect(socketAddress);
  //  }

  SocketError errorCode;
  bool completed = TryStartConnect(socket, socketAddress, errorCode);
  if (completed) {
    socket->RegisterConnectResult(errorCode);
    return errorCode;
  } else {
    return SocketError::WouldBlock;
  }
}


bool SocketPal::TryStartConnect(SafeSocketHandle::Ptr socket, Memory<byte> &socketAddress, SocketError &errorCode) {
  if (socket->m_is_disconnected) {
    errorCode = SocketError::IsConnected;
    return true;
  }
  auto rawSocketAddress = socketAddress.Span();
  Error err = interop::Sys::Connect(socket, rawSocketAddress.data(), rawSocketAddress.size());
  if (err == Error::RD_SUCCESS) {
    errorCode = SocketError::Success;
    return true;
  }

  if (err != Error::RD_EINPROGRESS) {
    errorCode = GetSocketErrorForErrorCode(err);
    return true;
  }

  errorCode = SocketError::Success;
  return false;
}

bool SocketPal::TryCompleteAccept(SafeSocketHandle::Ptr socket, Memory<byte> &socketAddress, int &socketAddressLen, int &acceptedFd, SocketError &errorCode) {
  int fd = 0;
  Error err;
  auto rawSocketAddress = socketAddress.Span().data();
  int sockAddrLen = socketAddress.Span().size();
  try {
    err = interop::Sys::Accept(socket, rawSocketAddress, &sockAddrLen, &fd);
    socketAddressLen = sockAddrLen;
  } catch (std::exception ex) {
    // The socket was closed, or is closing.
    errorCode = SocketError::OperationAborted;
    acceptedFd = -1;
    socketAddressLen = 0;
    return true;
  }

  if (err == Error::RD_SUCCESS) {
    errorCode = SocketError::Success;
    acceptedFd = fd;
    return true;
  }

  acceptedFd = -1;
  if (err != Error::RD_EAGAIN && err != Error::RD_EWOULDBLOCK) {
    errorCode = GetSocketErrorForErrorCode(err);
    return true;
  }

  errorCode = SocketError::Success;
  return false;
}

bool SocketPal::TryCompleteConnect(SafeSocketHandle::Ptr socket, SocketError &errorCode) {
  Error socketError;
  Error err;
  try {
    // Due to fd recyling, TryCompleteConnect may be called when there was a write event
    // for the previous socket that used the fd.
    // The SocketErrorOption in that case is the same as for a successful connect.
    // To filter out these false events, we check whether the socket is writable, before
    // reading the socket option.
    PollEvents outEvents;
    err = interop::Sys::Poll(socket, PollEvents::RD_POLLOUT, 0, outEvents);
    if (err == Error::RD_SUCCESS) {
      if (outEvents == PollEvents::RD_POLLNONE) {
        socketError = Error::RD_EINPROGRESS;
      } else {
        err = interop::Sys::GetSocketErrorOption(socket, socketError);
      }
    }
  } catch (std::exception ex) {
    // The socket was closed, or is closing.
    errorCode = SocketError::OperationAborted;
    return true;
  }

  if (err != Error::RD_SUCCESS) {
    errorCode = SocketError::Error;
    return true;
  }

  if (socketError == Error::RD_SUCCESS) {
    errorCode = SocketError::Success;
    return true;
  } else if (socketError == Error::RD_EINPROGRESS) {
    errorCode = SocketError::Success;
    return false;
  }

  errorCode = GetSocketErrorForErrorCode(socketError);
  return true;
}


NET_NAMESPACE_END
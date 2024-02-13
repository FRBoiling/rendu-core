/*
* Created by boil on 2023/11/2.
*/

#include "socket.h"

#include "address/dns.h"
#include "address/socket_address_pal.h"

#include "endpoint/dns_end_point.h"
#include "endpoint/ip_end_point.h"
#include "endpoint/unix_domain_socket_end_point.h"
#include "socket_async_event_args.h"
#include "socket_exception.h"
#include "socket_exception_factory.h"
#include "socket_pal.h"
#include "socket_protocol_support_pal.h"
#include "socket_exception.h"

NET_NAMESPACE_BEGIN

Socket::Socket(SocketType socketType, ProtocolType protocolType)
    : Socket(Socket::OSSupportsIPv6() ? AddressFamily::InterNetworkV6 : AddressFamily::InterNetwork, socketType, protocolType) {

  if (!Socket::OSSupportsIPv6())
    return;
  SetDualMode(true);
}

Socket::Socket(AddressFamily address_family, SocketType socket_type, ProtocolType protocol_type)
    : m_address_family(address_family), m_socket_type(socket_type), m_protocol_type(protocol_type) {

  SocketError socket_error = SocketPal::CreateSocket(m_address_family, m_socket_type, m_protocol_type, m_handle);
  if (socket_error != SocketError::Success)
    throw new SocketException(socket_error);
}

Socket::~Socket() { Close(); }

Socket::Socket(SafeSocketHandle::Ptr handle, bool loadPropertiesFromHandle)
    : m_handle(handle), m_address_family(AddressFamily::Unknown), m_socket_type(SocketType::Unknown), m_protocol_type(ProtocolType::Unknown) {
  if (!loadPropertiesFromHandle)
    return;
  bool isSocket = false;
  Socket::LoadSocketTypeFromHandle(handle, m_address_family, m_socket_type, m_protocol_type, m_will_block_internal, m_is_listening, isSocket);
  if (!isSocket)
    return;
  std::byte stack_buffer[SocketAddressPal::MaxAddressSize];
  std::span<std::byte> buffer(stack_buffer, SocketAddressPal::MaxAddressSize);
  int bufferLength = buffer.size();
  // 其他代码
  if (SocketPal::GetSockName(handle, buffer.data(), SocketAddressPal::MaxAddressSize) != SocketError::Success)
    return;

  switch (m_address_family) {
    case AddressFamily::InterNetwork: {
      m_right_endpoint = new IPEndPoint(
          new IPAddress(SocketAddressPal::GetIPv4Address(buffer.subspan(0, bufferLength)) & 0x0FFFFFFFF),
          SocketAddressPal::GetPort(buffer));
      break;
    }
    case AddressFamily::InterNetworkV6: {
      std::byte address_buffer[SocketAddressPal::IPv6AddressSize];
      std::span<std::byte> address(stack_buffer, SocketAddressPal::IPv6AddressSize);
      uint scope = 0;
      SocketAddressPal::GetIPv6Address(buffer.subspan(0, bufferLength), address, scope);
      m_right_endpoint = new IPEndPoint(
          new IPAddress(address, scope),
          SocketAddressPal::GetPort(buffer));
      break;
    }
    case AddressFamily::Unix: {
      m_right_endpoint = new UnixDomainSocketEndPoint(buffer.subspan(0, bufferLength));
      break;
    }
    default:
      break;
  }
  if (m_right_endpoint == nullptr)
    return;

  // 其他代码
  bufferLength = buffer.size();
  switch (SocketPal::GetPeerName(handle, buffer.data(), bufferLength)) {
    case SocketError::Success:
      switch (m_address_family) {
        case AddressFamily::InterNetwork: {
          m_remote_endpoint = new IPEndPoint(
              new IPAddress(SocketAddressPal::GetIPv4Address(buffer.subspan(0, bufferLength)) & 0x0FFFFFFFF),
              SocketAddressPal::GetPort(buffer));
          break;
        }
        case AddressFamily::InterNetworkV6: {
          std::byte address_buffer[SocketAddressPal::IPv6AddressSize];
          std::span<std::byte> address(stack_buffer, SocketAddressPal::IPv6AddressSize);
          uint scope = 0;
          SocketAddressPal::GetIPv6Address(buffer.subspan(0, bufferLength), address, scope);
          m_remote_endpoint = new IPEndPoint(
              new IPAddress(address, scope),
              SocketAddressPal::GetPort(buffer));
          break;
        }
        case AddressFamily::Unix: {
          m_remote_endpoint = new UnixDomainSocketEndPoint(buffer.subspan(0, bufferLength));
          break;
        }
        default:
          break;
      }
      m_is_connected = true;
      break;
    case SocketError::InvalidArgument:
      break;
    default:
      break;
  }
}

void Socket::Close() {
  if (m_handle) {
    m_handle->CloseSocket();
    m_handle.reset();
  }
}

bool Socket::SupportsIPv4() { return OSSupportsIPv4(); }
bool Socket::SupportsIPv6() { return OSSupportsIPv6(); }
bool Socket::OSSupportsIPv4() { return SocketProtocolSupportPal::OSSupportsIPv4(); }
bool Socket::OSSupportsIPv6() { return SocketProtocolSupportPal::OSSupportsIPv6(); }
bool Socket::OSSupportsUnixDomainSockets() { return SocketProtocolSupportPal::OSSupportsUnixDomainSockets(); }


SocketAddress *Socket::Serialize(EndPoint &remoteEP) {
  if (auto ipEndPoint = dynamic_cast<IPEndPoint *>(&remoteEP)) {
    auto address = ipEndPoint->GetAddress();
    if (address.GetFamily() == AddressFamily::InterNetwork && IsDualMode()) {
      throw std::invalid_argument("socket is dual mode");
    }
  } else if (dynamic_cast<DnsEndPoint *>(&remoteEP)) {
    throw std::invalid_argument("Invalid DnsEndPoint");
  }
  return remoteEP.Serialize();
}


bool Socket::GetDualMode() {
  return m_address_family == AddressFamily::InterNetworkV6 && GetSocketOption(SocketOptionLevel::IPv6, SocketOptionName::IPv6Only).has_value() == 0;
}

bool Socket::IsDualMode() {
  return GetDualMode();
}

void Socket::UpdateStatusAfterSocketOptionErrorAndThrowException(SocketError error, std::string_view callerName /*= ""*/) {
  bool disconnectOnFailure = error != SocketError::ProtocolOption && error != SocketError::OperationNotSupported;
  UpdateStatusAfterSocketErrorAndThrowException(error, disconnectOnFailure, callerName);
}

void Socket::UpdateStatusAfterSocketErrorAndThrowException(SocketError error, bool disconnectOnFailure, std::string_view callerName) {
  SocketException socketException(error);
  UpdateStatusAfterSocketError(socketException, disconnectOnFailure);
  throw socketException;
}

void Socket::UpdateStatusAfterSocketError(SocketException &socketException, bool disconnectOnFailure /*= true*/) {
  UpdateStatusAfterSocketError(socketException.GetSocketErrorCode(), disconnectOnFailure);
}

void Socket::UpdateStatusAfterSocketError(SocketError &errorCode, bool disconnectOnFailure /*= true*/) {
  if (!disconnectOnFailure || !m_is_connected ||
      !m_handle && (errorCode == SocketError::WouldBlock ||
                    errorCode == SocketError::IOPending ||
                    errorCode == SocketError::NoBufferSpaceAvailable ||
                    errorCode == SocketError::TimedOut)) {
    return;
  }

  SetToDisconnected();
}

void Socket::SetToDisconnected() {
  if (!m_is_connected)
    return;
  m_is_connected = false;
  m_is_disconnected = true;
}


LingerOption &Socket::GetLingerOpt() {
  LingerOption *optionValue = nullptr;
  SocketError lingerOption = SocketPal::GetLingerOption(m_handle, optionValue);
  if (lingerOption != SocketError::Success || !optionValue) {
    UpdateStatusAfterSocketOptionErrorAndThrowException(lingerOption, __func__);
  }
  return *optionValue;
}

MulticastOption &Socket::GetMulticastOpt(SocketOptionName optionName) {
  MulticastOption *optionValue = nullptr;
  SocketError multicastOption = SocketPal::GetMulticastOption(m_handle, optionName, optionValue);
  if (multicastOption != SocketError::Success)
    UpdateStatusAfterSocketOptionErrorAndThrowException(multicastOption, __func__);
  return *optionValue;
}

IPv6MulticastOption &Socket::GetIPv6MulticastOpt(SocketOptionName optionName) {
  IPv6MulticastOption *optionValue = nullptr;
  SocketError ipv6MulticastOption = SocketPal::GetIPv6MulticastOption(m_handle, optionName, optionValue);
  if (ipv6MulticastOption != SocketError::Success || !optionValue)
    UpdateStatusAfterSocketOptionErrorAndThrowException(ipv6MulticastOption, __func__);
  return *optionValue;
}

bool Socket::SetDualMode(bool is_dual_mode) {
  if (m_address_family != AddressFamily::InterNetworkV6)
    throw std::runtime_error("Unsupported address family. Only InterNetworkV6 is supported!");
  SetSocketOption(SocketOptionLevel::IPv6, SocketOptionName::IPv6Only, !is_dual_mode ? 1 : 0);
  return GetDualMode();
}

void Socket::SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue, bool silent) {
  if (silent && (m_handle->IsInvalid())) {
    //    RD_WARN("skipping the call");
    return;
  }
  SocketError error;
  error = SocketPal::SetSockOpt(m_handle, optionLevel, optionName, optionValue);
  //    RD_WARN("SetSockOpt returns errorCode:{}",error);
  if (optionName == SocketOptionName::PacketInformation && optionValue == 0 && error == SocketError::Success)
    //TODO:BOIL
    //this._receivingPacketInformation = false;
    if (silent || error == SocketError::Success)
      return;
  UpdateStatusAfterSocketOptionErrorAndThrowException(error, __func__);
}

void Socket::SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue) {
  SetSocketOption(optionLevel, optionName, optionValue, false);
}

std::any Socket::GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName) {
  //  // 确保socket未被释放
  //  if(IsDisposed())
  //  {
  //    throw std::runtime_error("Socket has been disposed.");
  //  }

  if (optionLevel == SocketOptionLevel::Socket && optionName == SocketOptionName::Linger)
    return static_cast<std::any>(GetLingerOpt());
  if (optionLevel == SocketOptionLevel::IP &&
      (optionName == SocketOptionName::AddMembership || optionName == SocketOptionName::DropMembership))
    return static_cast<std::any>(GetMulticastOpt(optionName));
  if (optionLevel == SocketOptionLevel::IPv6 &&
      (optionName == SocketOptionName::AddMembership || optionName == SocketOptionName::DropMembership))
    return static_cast<std::any>(GetIPv6MulticastOpt(optionName));

  int optionValue;
  SocketError sockErr = SocketPal::GetSockOpt(m_handle, optionLevel, optionName, optionValue);
  std::cout << "GetSockOpt returns errorCode:" << static_cast<int>(sockErr) << std::endl;

  if (sockErr != SocketError::Success) {
    UpdateStatusAfterSocketOptionErrorAndThrowException(sockErr, "GetSocketOption");
  }

  return static_cast<std::any>(optionValue);
}

void Socket::Bind(EndPoint &localEP) {
  SocketAddress *socketAddress = Serialize(localEP);
  DoBind(localEP, *socketAddress);
}

void Socket::Listen(int backlog /*= std::numeric_limits<int>::max()*/) {
  SocketError error = SocketPal::Listen(m_handle, backlog);
  if (error != SocketError::Success) {
    UpdateStatusAfterSocketErrorAndThrowException(error, true, __func__);
  }
  m_is_listening = true;
}

void Socket::Connect(EndPoint &remoteEP) {
  //FIXME：
  SocketAddress *socketAddress = Serialize(remoteEP);
  DoConnect(remoteEP, *socketAddress);
}

void Socket::Connect(IPAddress &address, ushort port) {
  //FIXME：
  IPEndPoint endPoint(&address, port);
  Connect(endPoint);
}

void Socket::Connect(std::string &host, ushort port) {
  if (auto address = IPAddress::Parse(host)) {
    Connect(address.value(), port);
    return;
  }
  if (auto address = Dns::GetHostAddresses(host)) {
    Connect(address.value(), port);
  }
}

Socket::Ptr Socket::Accept() {
  if (m_right_endpoint == nullptr)
    throw std::invalid_argument("Invalid net_sockets_mustbind");
  if (!m_is_listening)
    throw std::invalid_argument("Invalid net_sockets_mustlisten");
  if (m_is_disconnected)
    throw std::invalid_argument("Invalid net_sockets_disconnectedAccept");
  ValidateBlockingMode();
  SocketAddress socketAddress(m_address_family);
  SafeSocketHandle::Ptr socket = nullptr;
  SocketError socketError;
  try {
    int socketAddressLen;
    socketError = SocketPal::Accept(m_handle, socketAddress.GetBuffer(), socketAddressLen, socket);
    socketAddress.SetSize(socketAddressLen);
  } catch (Exception ex) {
    throw;
  }
  if (socketError != SocketError::Success) {
    UpdateAcceptSocketErrorForDisposed(socketError);
    UpdateStatusAfterSocketErrorAndThrowException(socketError, true, __func__);
  }
  //  Socket::Ptr acceptSocket = CreateAcceptSocket(socket, m_right_endpoint->Create(socketAddress));
  return nullptr;
}

Socket::Ptr Socket::CreateAcceptSocket(SafeSocketHandle::Ptr fd, EndPoint &remoteEP) {
  UpdateAcceptSocket(std::make_shared<Socket>(fd, false), remoteEP);
  return nullptr;
}

Socket::Ptr Socket::UpdateAcceptSocket(Socket::Ptr socket, EndPoint &remoteEP) {
  socket->m_address_family = m_address_family;
  socket->m_socket_type = m_socket_type;
  socket->m_protocol_type = m_protocol_type;
  socket->m_remote_endpoint = &remoteEP;
  //TODO：
  //  socket->m_right_endpoint = !(m_right_endpoint is UnixDomainSocketEndPoint rightEndPoint) || rightEndPoint.BoundFileName == null ? this._rightEndPoint : (EndPoint) rightEndPoint.CreateUnboundEndPoint();
  //  socket->m_local_endpoint = !Socket.IsWildcardEndPoint(this._localEndPoint) ? this._localEndPoint : (EndPoint) null;
  socket->SetToConnected();
  socket->m_will_block = m_will_block;
  //TODO：
  //  socket->InternalSetBlocking(this._willBlock);
  return socket;
}

void Socket::ValidateBlockingMode() {
  if (m_will_block && !m_will_block_internal)
    throw std::invalid_argument("Invalid net_invasync");
}

void Socket::DoConnect(EndPoint &end_point_snapshot, SocketAddress &socket_address) {
  SocketError socketError;
  try {
    socketError = SocketPal::Connect(m_handle, socket_address.GetBuffer().Slice(0, socket_address.Size()));
  } catch (Exception &ex) {
    throw;
  }
  if (socketError != SocketError::Success) {
    UpdateConnectSocketErrorForDisposed(socketError);
    SocketException *socketException = SocketExceptionFactory::CreateSocketException((int) socketError, end_point_snapshot);
    UpdateStatusAfterSocketError(*socketException);
    throw *socketException;
  }
  m_pending_connect_right_endpoint = &end_point_snapshot;
  m_non_blocking_connect_in_progress = false;
  SetToConnected();
}

void Socket::UpdateLocalEndPointOnConnect() {
  if (!Socket::IsWildcardEndPoint(m_local_endpoint))
    return;
  m_local_endpoint = nullptr;
}

bool Socket::IsWildcardEndPoint(EndPoint *endPoint) {
  if (endPoint == nullptr)
    return false;
  auto ipEndPoint = dynamic_cast<IPEndPoint *>(endPoint);
  if (ipEndPoint == nullptr) {
    return false;
  }
  IPAddress address = ipEndPoint->GetAddress();
  return address == IPAddress::Any() || IPAddress::IPv6Any() == address;
}

void Socket::SetToConnected() {
  if (m_is_connected)
    return;
  m_is_connected = true;
  m_is_disconnected = false;
  if (m_right_endpoint == nullptr)
    m_right_endpoint = m_pending_connect_right_endpoint;
  m_pending_connect_right_endpoint = nullptr;
  UpdateLocalEndPointOnConnect();
}


void Socket::DoBind(EndPoint &end_point_snapshot, SocketAddress &socket_address) {
  IPEndPoint *ip_end_point = dynamic_cast<IPEndPoint *>(&end_point_snapshot);
  if (!ip_end_point || !Socket::OSSupportsIPv4() && ip_end_point->GetAddress().IsIPv4MappedToIPv6()) {
    throw std::runtime_error("Invalid Argument in DoBind");
  }
  SocketError error = SocketPal::Bind(m_handle, m_protocol_type, socket_address.GetBuffer().Slice(0, socket_address.Size()).Span());
  if (error != SocketError::Success) {
    throw std::runtime_error("Error in DoBind");
  }
  auto domainSocketEndPoint = dynamic_cast<UnixDomainSocketEndPoint *>(&end_point_snapshot);
  m_right_endpoint = domainSocketEndPoint ? domainSocketEndPoint->CreateBoundEndPoint() : &end_point_snapshot;
}

void Socket::SetTcpNoDelay(bool on) {
  //  int optval = on ? 1 : 0;
  //  ::setsockopt(m_socket_fd, IPPROTO_TCP, TCP_NODELAY,
  //               &optval, static_cast<socklen_t>(sizeof optval));
  //  // FIXME CHECK
}


bool Socket::AcceptAsync(SocketAsyncEventArgs *e) {
  CancellationToken token;
  return AcceptAsync(*e, token);
}

bool Socket::AcceptAsync(SocketAsyncEventArgs &e, CancellationToken &cancellationToken) {
  if (m_right_endpoint == nullptr) {
    throw std::logic_error("Must listen on a socket before accepting connections.");
  }
  if (!m_is_listening) {
    throw std::logic_error("Must listen on a socket before accepting connections.");
  }
  // Handle AcceptSocket property.
  SafeSocketHandle::Ptr acceptHandle;
  auto socket = GetOrCreateAcceptSocket(e.GetAcceptSocket(), true, "AcceptSocket", acceptHandle);
  e.SetAcceptSocket( *socket);
  // Prepare for and make the native call.
  e.StartOperationCommon(this, SocketAsyncOperation::Accept);
  e.StartOperationAccept();
  SocketError socketError;
  try {
    socketError = e.DoOperationAccept(this, m_handle, acceptHandle, cancellationToken);
  } catch (Exception &ex) {
    // Clear in-use flag on event args object.
    e.Complete();
    throw;
  }
  return socketError == SocketError::IOPending;
}

std::optional<Socket*> Socket::GetOrCreateAcceptSocket(Socket *acceptSocket, bool checkDisconnected, string propertyName, SafeSocketHandle::Ptr handle) {
  if (acceptSocket != nullptr) {
    if (acceptSocket->m_handle->HasShutdownSend()) {
      throw new SocketException(SocketError::InvalidArgument);
    }

    if (acceptSocket->m_right_endpoint != nullptr && (!checkDisconnected || !acceptSocket->m_is_disconnected)) {
      throw std::invalid_argument("GetOrCreateAcceptSocket fail!");
    }
  }
  handle = nullptr;
  return acceptSocket;
}


bool Socket::ConnectAsync(SocketAsyncEventArgs *e) {
  //TODO:Boil
  return false;
}


bool Socket::ReceiveAsync(SocketAsyncEventArgs *e) {
  //TODO:Boil
  return false;
}

bool Socket::SendAsync(SocketAsyncEventArgs *e) {
  //TODO:Boil
  return false;
}

void Socket::LoadSocketTypeFromHandle(
    SafeSocketHandle::Ptr handle,
    AddressFamily &addressFamily,
    SocketType &socketType,
    ProtocolType &protocolType,
    bool &blocking,
    bool &isListening,
    bool &isSocket) {
  //TODO:Boil
}

bool Socket::IsConnectionOriented() {
  return m_socket_type == SocketType::Stream;
}

void Socket::UpdateReceiveSocketErrorForDisposed(SocketError &socket_error, int bytesTransferred) {
  // We use bytesTransferred for checking IsDisposed.
  // When there is a SocketError, bytesTransferred is zero.
  // An interrupted UDP receive on Linux returns SocketError.Success and bytesTransferred zero.
  if (bytesTransferred == 0 && IsDisposed()) {
    socket_error = IsConnectionOriented() ? SocketError::ConnectionAborted : SocketError::Interrupted;
  }
}

void Socket::UpdateSendSocketErrorForDisposed(SocketError &socket_error) {
  if (IsDisposed()) {
    socket_error = IsConnectionOriented() ? SocketError::ConnectionAborted : SocketError::Interrupted;
  }
}

void Socket::UpdateConnectSocketErrorForDisposed(SocketError &socket_error) {
  if (IsDisposed()) {
    socket_error = SocketError::NotSocket;
  }
}

void Socket::UpdateAcceptSocketErrorForDisposed(SocketError &socket_error) {
  if (IsDisposed()) {
    socket_error = SocketError::Interrupted;
  }
}

bool Socket::IsDisposed() {
  //FIXME:
  return false;
}


NET_NAMESPACE_END
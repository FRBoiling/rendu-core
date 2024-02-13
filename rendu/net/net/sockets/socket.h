/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_NET_SOCKET_H
#define RENDU_NET_SOCKET_H

#include "i_socket_info.h"
#include "socket_type.h"

#include "socket_exception.h"

#include "socket_error.h"
#include "socket_pal.h"

NET_NAMESPACE_BEGIN

class SocketAsyncEventArgs;

class Socket : public std::enable_shared_from_this<Socket>, public ISocketInfo, public NonCopyable {
public:
  using Ptr = std::shared_ptr<Socket>;

public:
  Socket(SocketType socketType, ProtocolType protocolType);
  Socket(AddressFamily address_family, SocketType socket_type, ProtocolType protocol_type);
  Socket(SafeSocketHandle::Ptr handle, bool loadPropertiesFromHandle);

  ~Socket();

private:
  static void LoadSocketTypeFromHandle(
      SafeSocketHandle::Ptr handle,
      AddressFamily &addressFamily,
      SocketType &socketType,
      ProtocolType &protocolType,
      bool &blocking,
      bool &isListening,
      bool &isSocket);

public:
  static bool SupportsIPv4();
  static bool SupportsIPv6();
  static bool OSSupportsIPv4();
  static bool OSSupportsIPv6();
  static bool OSSupportsUnixDomainSockets();

  SocketAddress *Serialize(EndPoint &remoteEP);

  EndPoint &GetLocalEndPoint() override { return *m_local_endpoint; };

  EndPoint &GetRemoteEndPoint() override { return *m_remote_endpoint; };

  void Close();

  bool GetDualMode();
  bool SetDualMode(bool is_dual_mode);

  void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue, bool silent);
  void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue);


  void UpdateStatusAfterSocketOptionErrorAndThrowException(SocketError error, std::string_view callerName = nullptr);
  void UpdateStatusAfterSocketErrorAndThrowException(SocketError error, bool disconnectOnFailure, std::string_view callerName);
  void UpdateStatusAfterSocketError(SocketException &socketException, bool disconnectOnFailure = true);
  void UpdateStatusAfterSocketError(SocketError &errorCode, bool disconnectOnFailure = true);

  void SetRemoteEndPoint(EndPoint &remote_end_point);

  void Bind(EndPoint &localEP);
  void Listen(int backlog = std::numeric_limits<int>::max());

  void Connect(EndPoint &remoteEP);
  void Connect(IPAddress &address, ushort port);
  void Connect(std::string &host, ushort port);

  Socket::Ptr Accept();


  bool AcceptAsync(SocketAsyncEventArgs *e);

  bool ConnectAsync(SocketAsyncEventArgs *e);

  bool ReceiveAsync(SocketAsyncEventArgs *e);
  bool SendAsync(SocketAsyncEventArgs *e);


  ///
  /// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
  ///
  void SetTcpNoDelay(bool on);

  std::any GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName);

  LingerOption &GetLingerOpt();
  MulticastOption &GetMulticastOpt(SocketOptionName optionName);
  IPv6MulticastOption &GetIPv6MulticastOpt(SocketOptionName optionName);

private:
  bool IsDualMode();
  void DoBind(EndPoint &end_point_snapshot, SocketAddress &socket_address);
  void DoConnect(EndPoint &end_point_snapshot, SocketAddress &socket_address);
  void SetToConnected();
  void SetToDisconnected();

  bool IsConnectionOriented();
  bool IsDisposed();
  void UpdateReceiveSocketErrorForDisposed(SocketError &socket_error, int bytesTransferred);
  void UpdateSendSocketErrorForDisposed(SocketError &socket_error);
  void UpdateConnectSocketErrorForDisposed(SocketError &socket_error);
  void UpdateAcceptSocketErrorForDisposed(SocketError &socket_error);

  void UpdateLocalEndPointOnConnect();
  void ValidateBlockingMode();


  Socket::Ptr CreateAcceptSocket(SafeSocketHandle::Ptr fd, EndPoint &remoteEP);

  Socket::Ptr UpdateAcceptSocket(Socket::Ptr socket, EndPoint &remoteEP);

  static bool IsWildcardEndPoint(EndPoint *endPoint);

  bool AcceptAsync(SocketAsyncEventArgs &e, CancellationToken &cancellationToken);
  bool ConnectAsync(SocketAsyncEventArgs &e, CancellationToken *cancellationToken);
  std::optional<Socket *> GetOrCreateAcceptSocket(Socket *acceptSocket, bool checkDisconnected, string propertyName, SafeSocketHandle::Ptr handle);


private:
  SafeSocketHandle::Ptr m_handle;
  bool m_is_listening;
  bool m_is_connected;
  bool m_is_disconnected;

  bool m_will_block;
  bool m_will_block_internal;

  AddressFamily m_address_family;
  SocketType m_socket_type;
  ProtocolType m_protocol_type;
  EndPoint *m_right_endpoint;
  EndPoint *m_remote_endpoint;
  EndPoint *m_local_endpoint;

  EndPoint *m_pending_connect_right_endpoint;

  bool m_non_blocking_connect_in_progress;
};

NET_NAMESPACE_END

#endif//RENDU_NET_SOCKET_H

/*
* Created by boil on 2024/2/5.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_ERROR_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_ERROR_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

 enum class SocketError
{
  /// <summary>An unspecified <see cref="T:System.Net.Sockets.Socket" /> error has occurred.</summary>
  SocketError = -1, // 0xFFFFFFFF
  /// <summary>The <see cref="T:System.Net.Sockets.Socket" /> operation succeeded.</summary>
  Success = 0,
  /// <summary>The overlapped operation was aborted due to the closure of the <see cref="T:System.Net.Sockets.Socket" />.</summary>
  OperationAborted = 995, // 0x000003E3
  /// <summary>The application has initiated an overlapped operation that cannot be completed immediately.</summary>
  IOPending = 997, // 0x000003E5
  /// <summary>A blocking <see cref="T:System.Net.Sockets.Socket" /> call was canceled.</summary>
  Interrupted = 10004, // 0x00002714
  /// <summary>An attempt was made to access a <see cref="T:System.Net.Sockets.Socket" /> in a way that is forbidden by its access permissions.</summary>
  AccessDenied = 10013, // 0x0000271D
  /// <summary>An invalid pointer address was detected by the underlying socket provider.</summary>
  Fault = 10014, // 0x0000271E
  /// <summary>An invalid argument was supplied to a <see cref="T:System.Net.Sockets.Socket" /> member.</summary>
  InvalidArgument = 10022, // 0x00002726
  /// <summary>There are too many open sockets in the underlying socket provider.</summary>
  TooManyOpenSockets = 10024, // 0x00002728
  /// <summary>An operation on a nonblocking socket cannot be completed immediately.</summary>
  WouldBlock = 10035, // 0x00002733
  /// <summary>A blocking operation is in progress.</summary>
  InProgress = 10036, // 0x00002734
  /// <summary>The nonblocking <see cref="T:System.Net.Sockets.Socket" /> already has an operation in progress.</summary>
  AlreadyInProgress = 10037, // 0x00002735
  /// <summary>A <see cref="T:System.Net.Sockets.Socket" /> operation was attempted on a non-socket.</summary>
  NotSocket = 10038, // 0x00002736
  /// <summary>A required address was omitted from an operation on a <see cref="T:System.Net.Sockets.Socket" />.</summary>
  DestinationAddressRequired = 10039, // 0x00002737
  /// <summary>The datagram is too long.</summary>
  MessageSize = 10040, // 0x00002738
  /// <summary>The protocol type is incorrect for this <see cref="T:System.Net.Sockets.Socket" />.</summary>
  ProtocolType = 10041, // 0x00002739
  /// <summary>An unknown, invalid, or unsupported option or level was used with a <see cref="T:System.Net.Sockets.Socket" />.</summary>
  ProtocolOption = 10042, // 0x0000273A
  /// <summary>The protocol is not implemented or has not been configured.</summary>
  ProtocolNotSupported = 10043, // 0x0000273B
  /// <summary>The support for the specified socket type does not exist in this address family.</summary>
  SocketNotSupported = 10044, // 0x0000273C
  /// <summary>The address family is not supported by the protocol family.</summary>
  OperationNotSupported = 10045, // 0x0000273D
  /// <summary>The protocol family is not implemented or has not been configured.</summary>
  ProtocolFamilyNotSupported = 10046, // 0x0000273E
  /// <summary>The address family specified is not supported. This error is returned if the IPv6 address family was specified and the IPv6 stack is not installed on the local machine. This error is returned if the IPv4 address family was specified and the IPv4 stack is not installed on the local machine.</summary>
  AddressFamilyNotSupported = 10047, // 0x0000273F
  /// <summary>Only one use of an address is normally permitted.</summary>
  AddressAlreadyInUse = 10048, // 0x00002740
  /// <summary>The selected IP address is not valid in this context.</summary>
  AddressNotAvailable = 10049, // 0x00002741
  /// <summary>The network is not available.</summary>
  NetworkDown = 10050, // 0x00002742
  /// <summary>No route to the remote host exists.</summary>
  NetworkUnreachable = 10051, // 0x00002743
  /// <summary>The application tried to set <see cref="F:System.Net.Sockets.SocketOptionName.KeepAlive" /> on a connection that has already timed out.</summary>
  NetworkReset = 10052, // 0x00002744
  /// <summary>The connection was aborted by .NET or the underlying socket provider.</summary>
  ConnectionAborted = 10053, // 0x00002745
  /// <summary>The connection was reset by the remote peer.</summary>
  ConnectionReset = 10054, // 0x00002746
  /// <summary>No free buffer space is available for a <see cref="T:System.Net.Sockets.Socket" /> operation.</summary>
  NoBufferSpaceAvailable = 10055, // 0x00002747
  /// <summary>The <see cref="T:System.Net.Sockets.Socket" /> is already connected.</summary>
  IsConnected = 10056, // 0x00002748
  /// <summary>The application tried to send or receive data, and the <see cref="T:System.Net.Sockets.Socket" /> is not connected.</summary>
  NotConnected = 10057, // 0x00002749
  /// <summary>A request to send or receive data was disallowed because the <see cref="T:System.Net.Sockets.Socket" /> has already been closed.</summary>
  Shutdown = 10058, // 0x0000274A
  /// <summary>The connection attempt timed out, or the connected host has failed to respond.</summary>
  TimedOut = 10060, // 0x0000274C
  /// <summary>The remote host is actively refusing a connection.</summary>
  ConnectionRefused = 10061, // 0x0000274D
  /// <summary>The operation failed because the remote host is down.</summary>
  HostDown = 10064, // 0x00002750
  /// <summary>There is no network route to the specified host.</summary>
  HostUnreachable = 10065, // 0x00002751
  /// <summary>Too many processes are using the underlying socket provider.</summary>
  ProcessLimit = 10067, // 0x00002753
  /// <summary>The network subsystem is unavailable.</summary>
  SystemNotReady = 10091, // 0x0000276B
  /// <summary>The version of the underlying socket provider is out of range.</summary>
  VersionNotSupported = 10092, // 0x0000276C
  /// <summary>The underlying socket provider has not been initialized.</summary>
  NotInitialized = 10093, // 0x0000276D
  /// <summary>A graceful shutdown is in progress.</summary>
  Disconnecting = 10101, // 0x00002775
  /// <summary>The specified class was not found.</summary>
  TypeNotFound = 10109, // 0x0000277D
  /// <summary>No such host is known. The name is not an official host name or alias.</summary>
  HostNotFound = 11001, // 0x00002AF9
  /// <summary>The name of the host could not be resolved. Try again later.</summary>
  TryAgain = 11002, // 0x00002AFA
  /// <summary>The error is unrecoverable or the requested database cannot be located.</summary>
  NoRecovery = 11003, // 0x00002AFB
  /// <summary>The requested name or IP address was not found on the name server.</summary>
  NoData = 11004, // 0x00002AFC
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_ERROR_H_

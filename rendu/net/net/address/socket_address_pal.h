/*
* Created by boil on 2024/2/17.
*/

#ifndef RENDU_NET_NET_ADDRESS_SOCKET_ADDRESS_PAL_H_
#define RENDU_NET_NET_ADDRESS_SOCKET_ADDRESS_PAL_H_

#include "net_define.h"

#include "address/address_family.h"
#include "address/socket_address.h"
#include "interop/sys.h"

NET_NAMESPACE_BEGIN

class SocketAddressPal {
public:
  static int IPv4AddressSize;
  static int IPv6AddressSize;
  static int UdsAddressSize;
  static int MaxAddressSize;

  static void Initialize() {
  }

  static void ThrowOnFailure(interop::Error err);

  static AddressFamily GetAddressFamily(std::span<byte> buffer);

  static void SetAddressFamily(std::span<byte> buffer, AddressFamily family);

  static ushort GetPort(std::span<byte> buffer);

  static void SetPort(std::span<byte> buffer, ushort port);

  static uint GetIPv4Address(std::span<byte> buffer);

  static void GetIPv6Address(std::span<byte> buffer, std::span<byte> address, uint &scope);

  static void SetIPv4Address(std::span<byte> buffer, uint address);

  static void SetIPv4Address(std::span<byte> buffer, byte *address);

  static void SetIPv6Address(std::span<byte> buffer, std::span<byte> address, uint scope);

  static void SetIPv6Address(std::span<byte> buffer, byte *address, int addressLength, uint scope);

  static void Clear(std::span<byte> buffer);
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_ADDRESS_SOCKET_ADDRESS_PAL_H_

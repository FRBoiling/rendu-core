/*
* Created by boil on 2024/2/17.
*/

#include "socket_address_pal.h"
#include <algorithm>

NET_NAMESPACE_BEGIN

int SocketAddressPal::IPv6AddressSize = 28;
int SocketAddressPal::IPv4AddressSize = 16;
int SocketAddressPal::UdsAddressSize = 28;
int SocketAddressPal::MaxAddressSize = SocketAddressPal::IPv6AddressSize;

void SocketAddressPal::ThrowOnFailure(interop::Error err) {
  if (err == interop::Error::RD_SUCCESS)
    return;
  if (err == interop::Error::RD_EAFNOSUPPORT)
    throw std::runtime_error("Not supported");
  if (err == interop::Error::RD_EFAULT)
    throw std::out_of_range("");
  throw std::runtime_error("Not supported");
}

AddressFamily SocketAddressPal::GetAddressFamily(std::span<byte> buffer) {
  AddressFamily family;
  interop::Error err = interop::Sys::GetAddressFamily(buffer.data(), buffer.size(), (int &) family);
  ThrowOnFailure(err);
  return family;
}

void SocketAddressPal::SetAddressFamily(std::span<byte> buffer, AddressFamily family) {
  if (family != AddressFamily::Unknown) {
    interop::Error err = interop::Sys::SetAddressFamily(buffer.data(), buffer.size(), (int) family);
    ThrowOnFailure(err);
  }
}

ushort SocketAddressPal::GetPort(std::span<byte> buffer) {
  ushort port;
  interop::Error err = interop::Sys::GetPort(buffer.data(), buffer.size(), &port);
  ThrowOnFailure(err);
  return port;
}

void SocketAddressPal::SetPort(std::span<byte> buffer, ushort port) {
  interop::Error err = interop::Sys::SetPort(buffer.data(), buffer.size(), port);
  ThrowOnFailure(err);
}

uint SocketAddressPal::GetIPv4Address(std::span<byte> buffer) {
  uint ipAddress;
  interop::Error err = interop::Sys::GetIPv4Address(buffer.data(), buffer.size(), &ipAddress);
  ThrowOnFailure(err);
  return ipAddress;
}

void SocketAddressPal::GetIPv6Address(std::span<byte> buffer, std::span<byte> address, uint &scope) {
  uint localScope;
  interop::Error err = interop::Sys::GetIPv6Address(buffer.data(), buffer.size(), address.data(), address.size(), &localScope);
  ThrowOnFailure(err);
  scope = localScope;
}

void SocketAddressPal::SetIPv4Address(std::span<byte> buffer, uint address) {
  interop::Error err = interop::Sys::SetIPv4Address(buffer.data(), buffer.size(), address);
  ThrowOnFailure(err);
}

void SocketAddressPal::SetIPv4Address(std::span<byte> buffer, byte *address) {
  uint private_address = uint(address[0]) << 24 |
                         uint(address[1]) << 16 |
                         uint(address[2]) << 8 |
                         uint(address[3]);
  SetIPv4Address(buffer, private_address);
}

void SocketAddressPal::SetIPv6Address(std::span<byte> buffer, byte *address, int addressLength, uint scope) {
  interop::Error err = interop::Sys::SetIPv6Address(buffer.data(), buffer.size(), address, addressLength, scope);
  ThrowOnFailure(err);
}

void SocketAddressPal::SetIPv6Address(std::span<byte> buffer, std::span<byte> address, uint scope) {
  SetIPv6Address(buffer, address.data(), address.size(), scope);
}

void SocketAddressPal::Clear(std::span<byte> buffer) {
  AddressFamily family = GetAddressFamily(buffer);
  std::fill(buffer.begin(), buffer.end(), (byte) 0);
  // platforms where this matters (OSXLike & BSD) use uint8 for SA length
  buffer[0] = (byte) std::min((int) buffer.size(), 255);
  SetAddressFamily(buffer, family);
}


NET_NAMESPACE_END

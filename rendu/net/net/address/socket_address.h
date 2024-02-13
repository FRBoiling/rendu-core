/*
* Created by boil on 2024/2/12.
*/

#ifndef RENDU_NET_NET_ADDRESS_SOCKET_ADDRESS_H_
#define RENDU_NET_NET_ADDRESS_SOCKET_ADDRESS_H_

#include "address/address_family.h"
#include "address/ip_address.h"
#include <algorithm>

NET_NAMESPACE_BEGIN

class SocketAddress : public EqualityComparable<SocketAddress> {
public:
  static int IPv6AddressSize;
  static int IPv4AddressSize;
  static int UdsAddressSize;
  static int MaxAddressSize;

  int m_size;
private:
  byte *m_buffer;
  std::span<byte> m_buffer_span;

private:
  const int MinSize = 2;
  const int DataOffset = 2;

private:
  bool m_changed = true;
  int m_hash;

public:
  AddressFamily GetFamily();
  int Size() { return m_size; }

  Memory<byte> GetBuffer();

public:
  SocketAddress(AddressFamily family);
  SocketAddress(AddressFamily family, int size);

  SocketAddress(IPAddress ipAddress);
  SocketAddress(IPAddress ipaddress, int port);

  ~SocketAddress();

  static int GetMaximumAddressSize(AddressFamily addressFamily);
  IPAddress GetIPAddress();

  void CopyAddressSizeIntoBuffer();

  int GetAddressSizeOffset();
  //  void SetSize(IntPtr ptr) = > m_size = *(int *) (void *) ptr;

public:
  bool operator==(const SocketAddress &rhs) const noexcept;
  //  bool operator<(const SocketAddress &rhs) const noexcept;

  int GetHashCode();

  string ToString();

  int SetSize(int size);
  void SetFamily(AddressFamily family);
  void SetIP(const IPAddress &ipAddress);
  void SetPort(int port);
};

inline bool SocketAddress::operator==(const SocketAddress &rhs) const noexcept {
  if (m_size != rhs.m_size)
    return false;
  for (int offset = 0; offset < m_size; ++offset) {
    if ((int) m_buffer[offset] != (int) rhs.m_buffer[offset])
      return false;
  }
  return true;
}

//inline bool SocketAddress::operator<(const SocketAddress &rhs) const noexcept {
//  if (m_size != rhs.m_size)
//    return false;
//  for (int offset = 0; offset < m_size; ++offset) {
//    if ((int) m_buffer[offset] < (int) rhs.m_buffer[offset])
//      return false;
//  }
//  return true;
//}

NET_NAMESPACE_END

#endif//RENDU_NET_NET_ADDRESS_SOCKET_ADDRESS_H_

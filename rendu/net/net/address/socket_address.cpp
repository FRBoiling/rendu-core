/*
* Created by boil on 2024/2/12.
*/

#include "socket_address.h"
#include "socket_address_pal.h"
#include "sockets/socket_exception.h"

NET_NAMESPACE_BEGIN

int SocketAddress::IPv6AddressSize = SocketAddressPal::IPv6AddressSize;
int SocketAddress::IPv4AddressSize = SocketAddressPal::IPv4AddressSize;
int SocketAddress::UdsAddressSize = SocketAddressPal::UdsAddressSize;
int SocketAddress::MaxAddressSize = SocketAddressPal::MaxAddressSize;

SocketAddress::SocketAddress(AddressFamily family, int size) : m_size(size) {
  m_buffer = new byte[m_size];
  m_buffer[0] = (byte) m_size;
  m_buffer_span = std::span<byte>(m_buffer, m_size);
  SocketAddressPal::SetAddressFamily(m_buffer_span, family);
}

SocketAddress::SocketAddress(AddressFamily family) : SocketAddress(family, GetMaximumAddressSize(family)) {
}

SocketAddress::SocketAddress(IPAddress ipAddress) : SocketAddress(ipAddress.GetFamily(), GetMaximumAddressSize(ipAddress.GetFamily())) {
  SocketAddressPal::SetPort(m_buffer_span, 0);
  if (ipAddress.GetFamily() == AddressFamily::InterNetworkV6) {
    SocketAddressPal::SetIPv6Address(m_buffer_span, ipAddress.GetAddressBytes(), ipAddress.GetScopeId());
  } else {
    SocketAddressPal::SetIPv4Address(m_buffer_span, ipAddress.GetAddressBytes().data());
  }
}

SocketAddress::SocketAddress(IPAddress ipaddress, int port)
    : SocketAddress(ipaddress) {
  SocketAddressPal::SetPort(m_buffer_span, (ushort) port);
}

// 修改后的析构函数
SocketAddress::~SocketAddress() {
  if (m_buffer != nullptr) {
    delete[] m_buffer;
    m_buffer = nullptr;
  }
}


int SocketAddress::GetMaximumAddressSize(AddressFamily addressFamily) {
  int maximumAddressSize;
  switch (addressFamily) {
    case AddressFamily::Unix:
      maximumAddressSize = SocketAddress::UdsAddressSize;
      break;
    case AddressFamily::InterNetwork:
      maximumAddressSize = SocketAddress::IPv4AddressSize;
      break;
    case AddressFamily::InterNetworkV6:
      maximumAddressSize = SocketAddress::IPv6AddressSize;
      break;
    default:
      maximumAddressSize = SocketAddress::MaxAddressSize;
      break;
  }
  return maximumAddressSize;
}

void SocketAddress::CopyAddressSizeIntoBuffer() {
  m_buffer[sizeof(m_buffer) - sizeof(int)] = (byte) m_size;
  m_buffer[sizeof(m_buffer) - sizeof(int) + 1] = (byte) (m_size >> 8);
  m_buffer[sizeof(m_buffer) - sizeof(int) + 2] = (byte) (m_size >> 16);
  m_buffer[sizeof(m_buffer) - sizeof(int) + 3] = (byte) (m_size >> 24);
}

int SocketAddress::GetAddressSizeOffset() { return sizeof(m_buffer) - sizeof(int); };


int SocketAddress::GetHashCode() {
  if (m_changed) {
    m_changed = false;
    m_hash = 0;
    int num1 = Size() & -4;
    int index;
    for (index = 0; index < num1; index += 4)
      m_hash ^= (int) m_buffer[index] | (int) m_buffer[index + 1] << 8 | (int) m_buffer[index + 2] << 16 | (int) m_buffer[index + 3] << 24;
    if ((Size() & 3) != 0) {
      int num2 = 0;
      int num3 = 0;
      for (; index < Size(); ++index) {
        num2 |= (int) m_buffer[index] << num3;
        num3 += 8;
      }
      m_hash ^= num2;
    }
  }
  return m_hash;
}

int SocketAddress::SetSize(int size) {
  m_size = size;
  return m_size;
};

string SocketAddress::ToString() {
  //    StringBuilder stringBuilder = new StringBuilder();
  //    for (int offset = 2; offset < GeySize(); ++offset) {
  //      if (offset > 2)
  //        stringBuilder.Append(",");
  //      stringBuilder.Append(this[offset].ToString((IFormatProvider) NumberFormatInfo.InvariantInfo));
  //    }
  //    return Family.ToString() + ":" + GeySize().ToString((IFormatProvider) NumberFormatInfo.InvariantInfo) + ":{" + stringBuilder.ToString() + "}";
  return "";
}

IPAddress SocketAddress::GetIPAddress() {
  if (GetFamily() == AddressFamily::InterNetworkV6) {
    byte address[16];
    for (int index = 0; index < sizeof(byte *); ++index)
      address[index] = m_buffer[index + 8];
    Long scopeid = (Long) (((int) m_buffer[27] << 24) + ((int) m_buffer[26] << 16) + ((int) m_buffer[25] << 8) + (int) m_buffer[24]);
    return IPAddress(address, scopeid);
  }

  if (GetFamily() == AddressFamily::InterNetwork)
    return IPAddress((Long) ((int) m_buffer[4] & (int) BYTE_MAX | (int) m_buffer[5] << 8 & 65280 | (int) m_buffer[6] << 16 & 16711680 | (int) m_buffer[7] << 24) & (Long) UINT_MAX);
  throw SocketException(SocketError::AddressFamilyNotSupported);
}

AddressFamily SocketAddress::GetFamily() {
  return SocketAddressPal::GetAddressFamily(m_buffer_span);
}

Memory<byte> SocketAddress::GetBuffer() {
  return Memory<byte>(m_buffer, sizeof(m_buffer));
}

NET_NAMESPACE_END
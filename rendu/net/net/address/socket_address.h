/*
* Created by boil on 2024/2/12.
*/

#ifndef RENDU_NET_NET_ADDRESS_SOCKET_ADDRESS_H_
#define RENDU_NET_NET_ADDRESS_SOCKET_ADDRESS_H_

#include "address_family.h"
#include "sockets/socket.h"
#include "exception/socket_exception.h"

NET_NAMESPACE_BEGIN

class SocketAddress {

  const int IPv6AddressSize = 28;
  const int IPv4AddressSize = 16;
  int m_Size;
  byte *m_Buffer;

private:
  const int WriteableOffset = 2;
  const int MaxSize = 32;
  bool m_changed = true;
  int m_hash;

public:
  AddressFamily GetFamily() { return (AddressFamily) ((int) m_Buffer[0] | (int) m_Buffer[1] << 8); };

  int Size() { return m_Size; }

  //  byte this[int offset] {
  //    get = > offset >= 0 && offset < this.Size ? this.m_Buffer[offset] : throw new IndexOutOfRangeException();
  //    set {
  //      if (offset < 0 || offset >= this.Size)
  //        throw new IndexOutOfRangeException();
  //      if ((int) this.m_Buffer[offset] != (int) value)
  //        this.m_changed = true;
  //      this.m_Buffer[offset] = value;
  //    }
  //  }

public:
  SocketAddress(AddressFamily family)
      : SocketAddress(family, 32) {
  }

  SocketAddress(AddressFamily family, int size) {
    m_Size = size >= 2 ? size : throw new ArgumentOutOfRangeException("size");
    m_Buffer = new byte[(size / sizeof(int) + 2) * sizeof(int)];
    m_Buffer[0] = (byte) family;
    m_Buffer[1] = (byte) ((uint) family >> 8);
  }

  SocketAddress(IPAddress ipAddress)
      : SocketAddress(ipAddress.GetAddressFamily(), ipAddress.GetAddressFamily() == AddressFamily::InterNetwork ? 16 : 28) {
    m_Buffer[2] = (byte) 0;
    m_Buffer[3] = (byte) 0;
    if (ipAddress.GetAddressFamily() == AddressFamily::InterNetworkV6) {
      m_Buffer[4] = (byte) 0;
      m_Buffer[5] = (byte) 0;
      m_Buffer[6] = (byte) 0;
      m_Buffer[7] = (byte) 0;
      Long scopeId = ipAddress.GetScopeId();
      m_Buffer[24] = (byte) scopeId;
      m_Buffer[25] = (byte) (scopeId >> 8);
      m_Buffer[26] = (byte) (scopeId >> 16);
      m_Buffer[27] = (byte) (scopeId >> 24);
      std::span<byte *> addressBytes = ipAddress.GetAddressBytes();
      for (int index = 0; index < addressBytes.size(); ++index)
        m_Buffer[8 + index] = *addressBytes[index];
    } else{
      int size = 0;
      ipAddress.TryWriteBytes(MemoryExtensions::AsSpan<byte>(m_Buffer, 4),size);
    }

  }

  SocketAddress(IPAddress ipaddress, int port)
      : SocketAddress(ipaddress) {
    m_Buffer[2] = (byte) (port >> 8);
    m_Buffer[3] = (byte) port;
  }

  IPAddress* GetIPAddress() {
    if (GetFamily() == AddressFamily::InterNetworkV6) {
      byte *address = new byte[16];
      for (int index = 0; index < sizeof(byte *); ++index)
        address[index] = m_Buffer[index + 8];
      Long scopeid = (Long) (((int) m_Buffer[27] << 24) + ((int) m_Buffer[26] << 16) + ((int) m_Buffer[25] << 8) + (int) m_Buffer[24]);
      return new IPAddress(address, scopeid);
    }
    if (GetFamily() == AddressFamily::InterNetwork)
      return new IPAddress((Long) ((int) m_Buffer[4] & (int) BYTE_MAX | (int) m_Buffer[5] << 8 & 65280 | (int) m_Buffer[6] << 16 & 16711680 | (int) m_Buffer[7] << 24) & (Long) UINT_MAX);
    throw SocketException(SocketError::AddressFamilyNotSupported);
  }

  IPEndPoint *GetIPEndPoint() { return new IPEndPoint(*GetIPAddress(), (int) m_Buffer[2] << 8 & 65280 | (int) m_Buffer[3]);}

  void CopyAddressSizeIntoBuffer() {
    m_Buffer[sizeof(m_Buffer) - sizeof(int)] = (byte) m_Size;
    m_Buffer[sizeof(m_Buffer) - sizeof(int) + 1] = (byte) (m_Size >> 8);
    m_Buffer[sizeof(m_Buffer) - sizeof(int) + 2] = (byte) (m_Size >> 16);
    m_Buffer[sizeof(m_Buffer) - sizeof(int) + 3] = (byte) (m_Size >> 24);
  }

  int GetAddressSizeOffset() { return sizeof(m_Buffer) - sizeof(int); };

  //  void SetSize(IntPtr ptr) = > m_Size = *(int *) (void *) ptr;

public:
  bool Equals(SocketAddress &comparand) {
    //    if (!(comparand is SocketAddress socketAddress) || Size != socketAddress.Size)
    //      return false;
    //    for (int offset = 0; offset < Size; ++offset) {
    //      if ((int) this[offset] != (int) socketAddress[offset])
    //        return false;
    //    }
    return true;
  }

  int GetHashCode() {
    if (m_changed) {
      m_changed = false;
      m_hash = 0;
      int num1 = Size() & -4;
      int index;
      for (index = 0; index < num1; index += 4)
        m_hash ^= (int) m_Buffer[index] | (int) m_Buffer[index + 1] << 8 | (int) m_Buffer[index + 2] << 16 | (int) m_Buffer[index + 3] << 24;
      if ((Size() & 3) != 0) {
        int num2 = 0;
        int num3 = 0;
        for (; index < Size(); ++index) {
          num2 |= (int) m_Buffer[index] << num3;
          num3 += 8;
        }
        m_hash ^= num2;
      }
    }
    return m_hash;
  }

  string ToString() {
    //    StringBuilder stringBuilder = new StringBuilder();
    //    for (int offset = 2; offset < GeySize(); ++offset) {
    //      if (offset > 2)
    //        stringBuilder.Append(",");
    //      stringBuilder.Append(this[offset].ToString((IFormatProvider) NumberFormatInfo.InvariantInfo));
    //    }
    //    return Family.ToString() + ":" + GeySize().ToString((IFormatProvider) NumberFormatInfo.InvariantInfo) + ":{" + stringBuilder.ToString() + "}";
    return "";
  }
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_ADDRESS_SOCKET_ADDRESS_H_

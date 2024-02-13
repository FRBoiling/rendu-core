/*
* Created by boil on 2023/10/15.
*/

#ifndef RENDU_IP_END_POINT_H
#define RENDU_IP_END_POINT_H

#include "net/net_define.h"

#include "end_point.h"

#include "address/ip_address.h"

NET_NAMESPACE_BEGIN

class IPEndPoint : public EndPoint,
                   public EqualityComparable<IPEndPoint>,
                   public LessThanComparable<IPEndPoint> {

public:
  const int MinPort = 0;
  const int MaxPort = 65535;

public:
  // Constructs to IPv4 end-point 0.0.0.0:0
  IPEndPoint() noexcept;
  IPEndPoint(Long address, ushort port) noexcept;
  IPEndPoint(IPAddress *ip_address, ushort port) noexcept;

public:
  static std::optional<IPEndPoint> Parse(const std::string_view host) noexcept;
  static std::optional<IPEndPoint> Parse(const std::string_view address, const ushort port) noexcept;

  std::string ToString() const override;

  IPAddress &GetAddress() noexcept;
  IPAddress &SetAddress(IPAddress &ip_address) noexcept;

  ushort GetPort() const noexcept;
  ushort SetPort(ushort port) noexcept;

  AddressFamily GetAddressFamily() override;

  bool operator==(const IPEndPoint &rhs) const noexcept;
  bool operator<(const IPEndPoint &rhs) const noexcept;

  SocketAddress *Serialize() override;
  EndPoint *Create(SocketAddress *socketAddress) override;

private:
  IPAddress m_ip_address;
  ushort m_port;
};

inline bool IPEndPoint::operator==(const IPEndPoint &rhs) const noexcept {
  return m_ip_address == rhs.m_ip_address && m_port == rhs.m_port;
}

inline bool IPEndPoint::operator<(const IPEndPoint &rhs) const noexcept {
  return m_ip_address < rhs.m_ip_address && m_port < rhs.m_port;
}

NET_NAMESPACE_END
#endif//RENDU_IP_END_POINT_H

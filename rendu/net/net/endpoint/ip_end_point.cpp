/*
* Created by boil on 2023/10/15.
*/

#include "ip_end_point.h"

NET_NAMESPACE_BEGIN

IPEndPoint::IPEndPoint() noexcept {
}

IPEndPoint::IPEndPoint(Long address, ushort port) noexcept {
}

IPEndPoint::IPEndPoint(IPAddress *ip_address, ushort port) noexcept : m_ip_address(*ip_address), m_port(port)  {
}

std::string IPEndPoint::ToString() const {
  return m_ip_address.ToString();
}

SocketAddress *IPEndPoint::Serialize() {
  //FIXME：
  return new SocketAddress(m_ip_address,m_port);
}

EndPoint *IPEndPoint::Create(SocketAddress *socketAddress) {
  //FIXME：
  return nullptr;
}

std::optional<IPEndPoint> IPEndPoint::Parse(const std::string_view host) noexcept {
  std::string_view s_view = host;
  std::size_t length1 = s_view.size();
  std::size_t length2 = s_view.rfind(':');
  if (length2 != std::string::npos) {
    if (s_view[length2 - 1] == ']') {
      length1 = length2;
    } else if (s_view.substr(0, length2).rfind(':') == std::string::npos) {
      length1 = length2;
    }
  }
  string ip_string = std::string(s_view.substr(0, length1));
  string port_string = std::string(s_view.substr(length1 + 1));
  auto address = IPAddress::Parse(ip_string);
  auto port = Convert::Parse<ushort>(port_string);
  if (!address.has_value() || !port.has_value()) {
    return std::nullopt;
  }
  return IPEndPoint(&*address, *port);
}

std::optional<IPEndPoint> IPEndPoint::Parse(const std::string_view ip_address, const ushort port) noexcept {
  auto address = IPAddress::Parse(ip_address);
  if (!address.has_value()) {
    return std::nullopt;
  }
  return IPEndPoint(&*address, port);
}

AddressFamily IPEndPoint::GetAddressFamily() {
  return m_ip_address.GetFamily();
}

IPAddress &IPEndPoint::GetAddress() noexcept {
  return m_ip_address;
}

IPAddress &IPEndPoint::SetAddress(IPAddress &ip_address) noexcept {
  return m_ip_address = ip_address;
}

NET_NAMESPACE_END

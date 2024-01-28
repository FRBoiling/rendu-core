/*
* Created by boil on 2023/10/16.
*/

#include "ip_v4_endpoint.h"

NET_NAMESPACE_BEGIN

  std::string IPv4EndPoint::ToString() const {
    auto s = m_address.ToString();
    s.push_back(':');
    s.append(std::to_string(m_port));
    return s;
  }

  std::optional<IPv4EndPoint> IPv4EndPoint::FromString(std::string_view host) noexcept {
    auto colonPos = host.find(':');
    if (colonPos == std::string_view::npos) {
      return std::nullopt;
    }

    auto address = IPv4Address::FromString(host.substr(0, colonPos));
    if (!address) {
      return std::nullopt;
    }

    auto port = Digit::StringToPort(host.substr(colonPos + 1));
    if (!port) {
      return std::nullopt;
    }
    return IPv4EndPoint{*address, *port};
  }

  std::optional<IPv4EndPoint> IPv4EndPoint::FromIpPort(std::string_view ip, uint16_t port) noexcept {
    auto address = IPv4Address::FromString(ip);
    if (!address) {
      return std::nullopt;
    }
    if (!port) {
      return std::nullopt;
    }
    return IPv4EndPoint{*address, port};
  }


NET_NAMESPACE_END
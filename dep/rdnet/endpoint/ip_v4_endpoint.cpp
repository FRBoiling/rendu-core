/*
* Created by boil on 2023/10/16.
*/

#include "ip_v4_endpoint.h"
#include "utils/end_point_utils.h"

RD_NAMESPACE_BEGIN

  string IPv4EndPoint::ToString() const {
    auto s = m_address.ToString();
    s.push_back(':');
    s.append(std::to_string(m_port));
    return s;
  }

  std::optional<IPv4EndPoint> IPv4EndPoint::FromString(std::string_view string) noexcept {
    auto colonPos = string.find(':');
    if (colonPos == std::string_view::npos) {
      return std::nullopt;
    }

    auto address = IPv4Address::FromString(string.substr(0, colonPos));
    if (!address) {
      return std::nullopt;
    }

    auto port = local::parse_port(string.substr(colonPos + 1));
    if (!port) {
      return std::nullopt;
    }
    return IPv4EndPoint{*address, *port};
  }

RD_NAMESPACE_END
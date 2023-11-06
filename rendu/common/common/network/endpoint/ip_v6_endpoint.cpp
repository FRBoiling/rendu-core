/*
* Created by boil on 2023/10/16.
*/

#include "ip_v6_endpoint.h"
#include "endpoint/end_point_utils.h"


RD_NAMESPACE_BEGIN

  std::string IPv6EndPoint::ToString() const {
    std::string result;
    result.push_back('[');
    result += m_address.ToString();
    result += "]:";
    result += std::to_string(m_port);
    return result;
  }

  std::optional<IPv6EndPoint>  IPv6EndPoint::FromString(std::string_view host) noexcept {

    // Shortest valid endpoint is "[::]:0"
    if (host.size() < 6) {
      return std::nullopt;
    }

    if (host[0] != '[') {
      return std::nullopt;
    }

    auto closeBracketPos = host.find("]:", 1);
    if (closeBracketPos == std::string_view::npos) {
      return std::nullopt;
    }

    auto address = IPv6Address::FromString(host.substr(1, closeBracketPos - 1));
    if (!address) {
      return std::nullopt;
    }

    auto port = local::parse_port(host.substr(closeBracketPos + 2));
    if (!port) {
      return std::nullopt;
    }

    return IPv6EndPoint{*address, *port};
  }

  std::optional<IPv6EndPoint> IPv6EndPoint::FromIpPort(std::string_view ip, uint16_t port) noexcept{
    // Shortest valid endpoint is "[::]:0"
    if (ip.size() < 6) {
      return std::nullopt;
    }

    if (ip[0] != '[') {
      return std::nullopt;
    }

    auto address = IPv6Address::FromString(ip);
    if (!address) {
      return std::nullopt;
    }

    if (!port) {
      return std::nullopt;
    }

    return IPv6EndPoint{*address, port};
  }

RD_NAMESPACE_END
/*
* Created by boil on 2023/10/16.
*/

#include "ip_v6_endpoint.h"
#include "utils/end_point_utils.h"


RD_NAMESPACE_BEGIN

  std::string IPv6EndPoint::ToString() const {
    std::string result;
    result.push_back('[');
    result += m_address.ToString();
    result += "]:";
    result += std::to_string(m_port);
    return result;
  }

  std::optional<IPv6EndPoint>
  IPv6EndPoint::FromString(std::string_view string) noexcept {

    // Shortest valid endpoint is "[::]:0"
    if (string.size() < 6) {
      return std::nullopt;
    }

    if (string[0] != '[') {
      return std::nullopt;
    }

    auto closeBracketPos = string.find("]:", 1);
    if (closeBracketPos == std::string_view::npos) {
      return std::nullopt;
    }

    auto address = IPv6Address::FromString(string.substr(1, closeBracketPos - 1));
    if (!address) {
      return std::nullopt;
    }

    auto port = local::parse_port(string.substr(closeBracketPos + 2));
    if (!port) {
      return std::nullopt;
    }

    return IPv6EndPoint{*address, *port};
  }

RD_NAMESPACE_END
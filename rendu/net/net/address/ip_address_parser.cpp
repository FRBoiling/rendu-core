/*
* Created by boil on 2024/2/19.
*/

#include "ip_address_parser.h"

NET_NAMESPACE_BEGIN

namespace detail {

  std::optional<IPAddress> IPAddressParser::Parse(const std::string_view ip_string, bool tryParse) {
    if (ip_string.find(':') == std::string::npos) {
      if (auto ipv4_address = IPv4Address::Parse(ip_string)) {
        return {*ipv4_address};
      }
    } else {
      if (auto ipv6_address = IPv6Address::Parse(ip_string)) {
        return {*ipv6_address};
      }
    }
    if (!tryParse) {
      throw std::runtime_error("Bad IP address");
    }
    return std::nullopt;
  }

}// namespace detail

NET_NAMESPACE_END
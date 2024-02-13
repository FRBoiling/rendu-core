/*
* Created by boil on 2023/10/16.
*/

#include "ip_address.h"
#include "ip_address_parser.h"

NET_NAMESPACE_BEGIN


AddressFamily IPAddress::GetFamily() {
  return IsIPv4() ? AddressFamily::InterNetwork : IsIPv6() ? AddressFamily::InterNetworkV6
                                                           : AddressFamily::Unknown;
}


std::span< byte> IPAddress::GetAddressBytes()  {
  if (IsIPv4()) {
    return m_ipv4.GetBytes();
  } else if (IsIPv6()) {
    return m_ipv6.GetBytes();
  } else {
    // Return an empty span for non-IPv4/IPv6 cases
    return std::span< byte>();
  }
};

bool IPAddress::IsIPv4MappedToIPv6() {
  //TODO:
  return false;
}

IPAddress *MapToIPv6() {
  //TODO:
  return nullptr;
}

std::optional<IPAddress> IPAddress::Parse(const std::string_view ip_string) {
  return detail::IPAddressParser::Parse(ip_string, false);
}

std::string IPAddress::ToString() const {
  return IsIPv4() ? m_ipv4.ToString() : m_ipv6.ToString();
}

NET_NAMESPACE_END

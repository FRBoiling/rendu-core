/*
* Created by boil on 2024/2/19.
*/

#ifndef RENDU_NET_NET_ADDRESS_DETAIL_IP_ADDRESS_PARSER_H_
#define RENDU_NET_NET_ADDRESS_DETAIL_IP_ADDRESS_PARSER_H_

#include "ip_address.h"

NET_NAMESPACE_BEGIN

namespace detail {

  class IPAddressParser {
  public:
    static std::optional<IPAddress> Parse(const std::string_view ip_string, bool tryParse);

  };

}// namespace detail


NET_NAMESPACE_END

#endif//RENDU_NET_NET_ADDRESS_DETAIL_IP_ADDRESS_PARSER_H_

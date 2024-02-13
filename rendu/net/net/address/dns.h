/*
* Created by boil on 2024/2/22.
*/

#ifndef RENDU_NET_NET_ADDRESS_DNS_H_
#define RENDU_NET_NET_ADDRESS_DNS_H_

#include "net_define.h"

#include "interop/sys.h"
#include "ip_address.h"

NET_NAMESPACE_BEGIN

class Dns {
public:
  static string GetHostName();

  static std::optional<IPAddress> GetHostAddresses(string hostNameOrAddress);
  static std::optional<IPAddress> GetHostAddresses(string hostNameOrAddress, AddressFamily family);

private:
  static std::optional<IPAddress> GetHostAddressesCore(std::string_view hostName, AddressFamily addressFamily, std::optional<Long> startingTimestamp = 0);

  static std::optional<IPAddress> GetHostEntryOrAddressesCore(std::string_view hostName, bool justAddresses, AddressFamily addressFamily, std::optional<Long> startingTimestamp = 0);
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_ADDRESS_DNS_H_

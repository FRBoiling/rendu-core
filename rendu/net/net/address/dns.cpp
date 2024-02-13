/*
* Created by boil on 2024/2/22.
*/

#include "dns.h"

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

NET_NAMESPACE_BEGIN

string Dns::GetHostName() {
  return interop::Sys::GetHostName();
}

std::optional<IPAddress> Dns::GetHostAddresses(string hostNameOrAddress) {
  return GetHostAddresses(hostNameOrAddress, AddressFamily::Unspecified);
}

std::optional<IPAddress> Dns::GetHostAddresses(string hostNameOrAddress, AddressFamily family) {
  if (auto address = IPAddress::Parse(hostNameOrAddress)) {
    if (address == IPAddress::Any() || address == IPAddress::IPv6Any()) {
      throw std::invalid_argument("net_invalid_ip_addr");
    }
    if (family != AddressFamily::Unspecified && address->GetFamily() != family) {
      return std::nullopt;
    }
    return address;
  }
  return Dns::GetHostAddressesCore(hostNameOrAddress, family, 0);
}

std::optional<IPAddress> Dns::GetHostAddressesCore(std::string_view hostName, AddressFamily addressFamily, std::optional<Long> startingTimestamp /*= 0*/) {
  return GetHostEntryOrAddressesCore(hostName, true, addressFamily, startingTimestamp);
}


std::optional<IPAddress> Dns::GetHostEntryOrAddressesCore(std::string_view hostName, bool justAddresses, AddressFamily addressFamily, std::optional<Long> startingTimestamp /* = 0*/) {

  int family = (addressFamily == AddressFamily::InterNetwork) ? AF_INET : (addressFamily == AddressFamily::InterNetworkV6) ? AF_INET6
                                                                                                                           : AF_UNSPEC;

  // Define the variables
  struct addrinfo hints {}, *res{};
  hints.ai_family = family;

#if defined(_WIN32)

  // Initialize Winsock
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
    // Handle error
  }

  // Use Winsock version
  GetAddrInfoW(reinterpret_cast<LPCWSTR>(hostName.data()), nullptr, &hints, &res);

#else

  // Use POSIX version
  getaddrinfo(hostName.data(), nullptr, &hints, &res);

#endif

  if (res == nullptr) {
    return std::nullopt;
  }

  IPAddress ipAddress{};// Parse the IP Address from `res` depending your IPAddress structure

#if defined(_WIN32)
  // Don't forget to cleanup
  FreeAddrInfoW(res);
  WSACleanup();
#else
  freeaddrinfo(res);
#endif

  return ipAddress;
}

NET_NAMESPACE_END

/*
* Created by boil on 2023/11/4.
*/

#ifndef RENDU_DNS_CACHE_H
#define RENDU_DNS_CACHE_H

#include "sock_ops.h"
#include <unordered_map>
#include <mutex>

RD_NAMESPACE_BEGIN

  class DnsItem {
  public:
    std::shared_ptr<struct addrinfo> addr_info;
    time_t create_time;
  };

  class DnsCache {
  public:
    static DnsCache &Instance() {
      static DnsCache instance;
      return instance;
    }

    bool getDomainIP(const char *host, sockaddr_storage &storage, int ai_family = AF_INET,
                     int ai_socktype = SOCK_STREAM, int ai_protocol = IPPROTO_TCP, int expire_sec = 60);

  private:
    std::shared_ptr<struct addrinfo> getCacheDomainIP(const char *host, int expireSec);

    void SetCacheDomainIP(const char *host, std::shared_ptr<struct addrinfo> addr);

    std::shared_ptr<struct addrinfo> getSystemDomainIP(const char *host);

    struct addrinfo *getPerferredAddress(struct addrinfo *answer, int ai_family, int ai_socktype, int ai_protocol);
  private:
    std::mutex _mtx;
    std::unordered_map <string, DnsItem> _dns_cache;
  };

RD_NAMESPACE_END


#endif //RENDU_DNS_CACHE_H

/*
* Created by boil on 2023/11/4.
*/

#ifndef RENDU_DNS_CACHE_H
#define RENDU_DNS_CACHE_H

#include "net_define.h"

#include <mutex>
#include <unordered_map>
#include "sockets/sock_ops.h"
#include "errno/errno.h"

NET_NAMESPACE_BEGIN

  class DnsItem {
public:
    std::shared_ptr<struct addrinfo> addr_info;
    DateTime create_time{DateTime::MinValue};
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
    std::unordered_map <std::string, DnsItem> _dns_cache;
  };

NET_NAMESPACE_END


#endif //RENDU_DNS_CACHE_H

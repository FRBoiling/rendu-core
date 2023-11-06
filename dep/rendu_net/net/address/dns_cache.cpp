/*
* Created by boil on 2023/11/4.
*/

#include "dns_cache.h"
#include "net/socket/socket_helper.h"
#include "logger/log.h"
#include "utils/rd_errno.h"

RD_NAMESPACE_BEGIN

using namespace std;

bool DnsCache::getDomainIP(const char *host, sockaddr_storage &storage, int ai_family, int ai_socktype,
                                  int ai_protocol, int expire_sec) {
  try {
    storage = SocketHelper::MakeSockAddr(host, 0);
    return true;
  } catch (...) {
    auto item = getCacheDomainIP(host, expire_sec);
    if (!item) {
      item = getSystemDomainIP(host);
      if (item) {
        SetCacheDomainIP(host, item);
      }
    }
    if (item) {
      auto addr = getPerferredAddress(item.get(), ai_family, ai_socktype, ai_protocol);
      memcpy(&storage, addr->ai_addr, addr->ai_addrlen);
    }
    return (bool) item;
  }
}

  std::shared_ptr<struct addrinfo> DnsCache::getCacheDomainIP(const char *host, int expireSec) {
    lock_guard <mutex> lck(_mtx);
    auto it = _dns_cache.find(host);
    if (it == _dns_cache.end()) {
      //没有记录
      return nullptr;
    }
    if (it->second.create_time + expireSec < time(nullptr)) {
      //已过期
      _dns_cache.erase(it);
      return nullptr;
    }
    return it->second.addr_info;
  }

  void DnsCache::SetCacheDomainIP(const char *host, std::shared_ptr<struct addrinfo> addr) {
    lock_guard <mutex> lck(_mtx);
    DnsItem item;
    item.addr_info = std::move(addr);
    item.create_time = time(nullptr);
    _dns_cache[host] = std::move(item);
  }

  std::shared_ptr<struct addrinfo> DnsCache::getSystemDomainIP(const char *host) {
    struct addrinfo *answer = nullptr;
    //阻塞式dns解析，可能被打断
    int ret = -1;
    do {
      ret = getaddrinfo(host, nullptr, nullptr, &answer);
    } while (ret == -1 && GetError(true) == EINTR);

    if (!answer) {
      LOG_WARN << "getaddrinfo failed: " << host;
      return nullptr;
    }
    return std::shared_ptr<struct addrinfo>(answer, freeaddrinfo);
  }

  struct addrinfo *
  DnsCache::getPerferredAddress(struct addrinfo *answer, int ai_family, int ai_socktype, int ai_protocol) {
    auto ptr = answer;
    while (ptr) {
      if (ptr->ai_family == ai_family && ptr->ai_socktype == ai_socktype && ptr->ai_protocol == ai_protocol) {
        return ptr;
      }
      ptr = ptr->ai_next;
    }
    return answer;
  }

RD_NAMESPACE_END
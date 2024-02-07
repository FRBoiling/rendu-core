/*
* Created by boil on 2023/11/4.
*/

#ifndef RENDU_SOCK_OPS_H
#define RENDU_SOCK_OPS_H

#include "net/net_define.h"

#include "posix_sock.h"
#include "win_sock.h"
#include "apple_sock.h"

NET_NAMESPACE_BEGIN

#define RD_NONE 0       /* No events registered. */
#define RD_READABLE 1   /* Fire when descriptor is readable. */
#define RD_WRITABLE 2   /* Fire when descriptor is writable. */
#define RD_BARRIER 4    /* With WRITABLE, never fire the event if the
                           READABLE event already fired in the same event
                           loop iteration. Useful when you want to persist
                           things to disk before sending replies, and want
                           to do that in a group fashion. */

#define SOCKET_DEFAULT_BUF_SIZE (256 * 1024)
#define TCP_KEEPALIVE_INTERVAL 30
#define TCP_KEEPALIVE_PROBE_TIMES 9
#define TCP_KEEPALIVE_TIME 120

  namespace SockOps {
    int Pipe(int fd[2]);

    int CreateNonblockingOrDie(sa_family_t family);

    void Close(int sfd);

    void ShutdownWrite(SOCKET sockfd);

    int BindSock6(int fd, uint16_t port, const char *ifr_ip);

    int BindSock4(int fd, uint16_t port, const char *ifr_ip);

    int BindSock(int fd, uint16_t port, const char *ifr_ip, int family);

    SOCKET Bind(uint16_t port, const char *ifr_ip, int family);

    void Listen(const SOCKET sfd, int back_log = 1024);

    int Listen(const uint16_t port, const char *local_ip, int back_log);


    SOCKET Accept(const SOCKET listen_sfd);
    SOCKET Accept(const SOCKET listen_sfd,struct sockaddr_storage& storage,socklen_t& addr_len) ;


    SOCKET Connect(const SOCKET sfd, struct sockaddr_storage &storage);
    SOCKET Connect(const char *remote_host, uint16_t remote_port);
    int Connect(const char *host, uint16_t port, bool async, const char *local_ip, uint16_t local_port);

    bool IsSelfConnect(int sockfd);


    ssize_t Read(int sockfd, void *buf, size_t count);

    ssize_t Readv(int sockfd, const struct iovec *iov, int iovcnt);


    ssize_t Write(int sockfd, const void *buf, size_t count);


    bool IsIPv4(const char *host);

    bool IsIPv6(const char *host);

    bool IsIP(const char *str);

    bool SupportIPv6();

    int SetIpv6Only(int fd, bool flag);


    socklen_t GetSockLen(const struct sockaddr *addr);
    socklen_t GetSockLen(int family) ;

    struct sockaddr_storage MakeSockAddr(const char *host, uint16_t port);


    int SetNoDelay(int fd, bool on = true);

    int SetNoSigpipe(int fd);

    int SetNoBlocked(int fd, bool noblock = true);

    int SetCloExec(int fd, bool on = true);

    int SetCloseWait(int sock, int second = 0);

    int SetReuseable(int fd, bool on = true, bool reuse_port = true);

    int SetBroadcast(int fd, bool on = true);

    int SetKeepAlive(int fd, bool on = true, int interval = TCP_KEEPALIVE_INTERVAL, int idle = TCP_KEEPALIVE_TIME,
                     int times = TCP_KEEPALIVE_PROBE_TIMES);


    int SetRecvBuf(int fd, int size = SOCKET_DEFAULT_BUF_SIZE);

    int SetSendBuf(int fd, int size = SOCKET_DEFAULT_BUF_SIZE);



    std::string InetNTop(int af, const void *addr);

    std::string InetNToa(const struct in_addr &addr);

    std::string InetNToa(const struct in6_addr &addr);

    std::string InetNToa(const struct sockaddr *addr);

    uint16_t InetPort(const struct sockaddr *addr);


    bool GetDomainIP(const char *host, uint16_t port, struct sockaddr_storage &addr,
                     int ai_family = AF_INET, int ai_socktype = SOCK_STREAM, int ai_protocol = IPPROTO_TCP,
                     int expire_sec = 60);


    int GetSockError(int fd);


    bool GetSockLocalAddr(int fd, struct sockaddr_storage &addr);

    bool GetSockPeerAddr(int fd, struct sockaddr_storage &addr);

    std::string GetLocalIP(int fd);

    std::string GetPeerIP(int fd);

    uint16_t GetLocalPort(int fd);

    uint16_t GetPeerPort(int fd);

    bool CheckIP(std::string &address, const std::string &ip);

    std::string GetLocalIP();


    int BindUdpSock(const uint16_t port, const char *local_ip, bool enable_reuse);

    int DissolveUdpSock(int fd);


    std::string get_ifr_ip(const char *if_name);

    std::string get_ifr_name(const char *local_ip);

    std::string get_ifr_mask(const char *if_name);

    std::string get_ifr_brdaddr(const char *if_name);

    bool in_same_lan(const char *myIp, const char *dstIp);

    void clearMulticastAllSocketOption(int socket);

    int SetMultiTTL(int fd, uint8_t ttl);

    int SetMultiLOOP(int fd, bool accept);

    int joinMultiAddr(int fd, const char *addr, const char *local_ip);

    int leaveMultiAddr(int fd, const char *addr, const char *local_ip);

    int joinMultiAddrFilter(int fd, const char *addr, const char *src_ip, const char *local_ip);

    int leaveMultiAddrFilter(int fd, const char *addr, const char *src_ip, const char *local_ip);

//    /**
//     * 获取网卡列表
//     * @return vector<map<ip:name> >
//     */
//    std::vector<std::map<std::string, std::string>> getInterfaceList();

  }

NET_NAMESPACE_END

#endif //RENDU_SOCK_OPS_H

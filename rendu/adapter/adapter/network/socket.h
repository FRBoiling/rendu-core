/*
* Created by boil on 2023/10/16.
*/

#ifndef RENDU_SOCKET_H
#define RENDU_SOCKET_H

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

#include "define.h"
#include "address_family.h"
#include "socket_type.h"
#include "protocol_type.h"
#include "socket_error.h"

RD_NAMESPACE_BEGIN

#ifndef _WIN32
/* For POSIX systems we use the standard BSD socket API. */
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#else
    /* For Windows we use winsock. */
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 /* To get WSAPoll etc. */
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stddef.h>
#include <errno.h>
#include <mstcpip.h>

#ifdef _MSC_VER
typedef long long ssize_t;
#endif

/* Emulate the parts of the BSD socket API that we need (override the winsock signatures). */
int win32_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
const char *win32_gai_strerror(int errcode);
void win32_freeaddrinfo(struct addrinfo *res);
SOCKET win32_socket(int domain, int type, int protocol);
int win32_ioctl(SOCKET fd, unsigned long request, unsigned long *argp);
int win32_bind(SOCKET sockfd, const struct sockaddr *addr, socklen_t addrlen);
int win32_connect(SOCKET sockfd, const struct sockaddr *addr, socklen_t addrlen);
int win32_getsockopt(SOCKET sockfd, int level, int optname, void *optval, socklen_t *optlen);
int win32_setsockopt(SOCKET sockfd, int level, int optname, const void *optval, socklen_t optlen);
int win32_close(SOCKET fd);
ssize_t win32_recv(SOCKET sockfd, void *buf, size_t len, int flags);
ssize_t win32_send(SOCKET sockfd, const void *buf, size_t len, int flags);
typedef ULONG nfds_t;
int win32_poll(struct pollfd *fds, nfds_t nfds, int timeout);

int win32_redisKeepAlive(SOCKET sockfd, int interval_ms);

#ifndef REDIS_SOCKCOMPAT_IMPLEMENTATION
#define getaddrinfo(node, service, hints, res) win32_getaddrinfo(node, service, hints, res)
#undef gai_strerror
#define gai_strerror(errcode) win32_gai_strerror(errcode)
#define freeaddrinfo(res) win32_freeaddrinfo(res)
#define socket(domain, type, protocol) win32_socket(domain, type, protocol)
#define ioctl(fd, request, argp) win32_ioctl(fd, request, argp)
#define bind(sockfd, addr, addrlen) win32_bind(sockfd, addr, addrlen)
#define connect(sockfd, addr, addrlen) win32_connect(sockfd, addr, addrlen)
#define getsockopt(sockfd, level, optname, optval, optlen) win32_getsockopt(sockfd, level, optname, optval, optlen)
#define setsockopt(sockfd, level, optname, optval, optlen) win32_setsockopt(sockfd, level, optname, optval, optlen)
#define close(fd) win32_close(fd)
#define recv(sockfd, buf, len, flags) win32_recv(sockfd, buf, len, flags)
#define send(sockfd, buf, len, flags) win32_send(sockfd, buf, len, flags)
#define poll(fds, nfds, timeout) win32_poll(fds, nfds, timeout)
#endif /* REDIS_SOCKCOMPAT_IMPLEMENTATION */
#endif /* _WIN32 */


    class IPEndPoint;

    class SocketAsyncEventArgs;

    class Socket {
    public:
      Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType);
      ~Socket();

    private:
      int32 m_addressFamily;
      int32 m_socketType;
      int32 m_protocolType;
      int32 m_socket_fd;
    public:
      IPEndPoint *GetRemoteEndPoint();

      void SetNoDelay(bool b);

      void Close();

      bool AcceptAsync(SocketAsyncEventArgs *pArgs);

      bool ConnectAsync(SocketAsyncEventArgs *pArgs);

      bool SendAsync(SocketAsyncEventArgs *pArgs);

      bool ReceiveAsync(SocketAsyncEventArgs *pArgs);

      void Bind(IPEndPoint &point);

      void Listen(int backlog);

    private:
      void Connect();
      void Accept();
    };

RD_NAMESPACE_END

#endif //RENDU_SOCKET_H

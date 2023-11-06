/*
* Created by boil on 2023/10/27.
*/
#ifdef _WIN32

<<<<<<<< HEAD:dep/toolkit/net/utils/win/win_sock.h
#ifndef RENDU_WIN_SOCK_H
#define RENDU_WIN_SOCK_H
========
#ifndef RENDU_COMMON_WIN_SOCK_H
#define RENDU_COMMON_WIN_SOCK_H
>>>>>>>> f6e2ef7 (✨ feat(框架): 整理项目结构):rendu/common/common/network/sockets/win_sock.h


/*
 * Some minor differences between sockets on various platforms.
 * We include whatever sockets are needed for Internet-protocol
 * socket access on UN*X and Windows.
 */
#include <rpcdce.h>

/* Need windef.h for defines used in winsock2.h under MingW32 */
#ifdef __MINGW32__
#include <windef.h>
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
/*
* Winsock doesn't have this POSIX type; it's used for the
* tv_usec value of struct timeval.
*/
typedef long suseconds_t;

/*!
 * \brief In Winsock, the error return if socket() fails is INVALID_SOCKET;
 * in UN*X, it's -1.
 * We define INVALID_SOCKET to be -1 on UN*X, so that it can be used on
 * both platforms.
 */
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

/* For Windows we use winsock. */
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 /* To get WSAPoll etc. */
#include <stddef.h>
#include <errno.h>
#include <mstcpip.h>

#ifdef _MSC_VER
typedef long long ssize_t;
#endif

#include "define.h"
RD_NAMESPACE_BEGIN

/* Emulate the parts of the BSD socket API that we need (override the winsock signatures). */
int win32_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
void win32_freeaddrinfo(struct addrinfo *res);
const char *win32_gai_strerror(int errcode);
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

RD_NAMESPACE_END

#endif /* _WIN32 */

<<<<<<<< HEAD:dep/toolkit/net/utils/win/win_sock.h
#endif //RENDU_WIN_SOCK_H
========
#endif //RENDU_COMMON_WIN_SOCK_H
>>>>>>>> f6e2ef7 (✨ feat(框架): 整理项目结构):rendu/common/common/network/sockets/win_sock.h

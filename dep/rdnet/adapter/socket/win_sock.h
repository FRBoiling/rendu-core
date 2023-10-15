/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_WIN_SOCK_H
#define RENDU_WIN_SOCK_H

#ifdef _WIN32
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

RD_NAMESPACE_END

#endif /* _WIN32 */

#endif //RENDU_WIN_SOCK_H

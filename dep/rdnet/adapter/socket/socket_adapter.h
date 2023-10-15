/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_SOCKET_ADAPTER_H
#define RENDU_SOCKET_ADAPTER_H

#include "define.h"

#ifndef _WIN32


#include "posix_sock.h"

RD_NAMESPACE_BEGIN
RD_NAMESPACE_END

#else

#include "win_sock.h"

RD_NAMESPACE_BEGIN

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

#endif

#endif //RENDU_SOCKET_ADAPTER_H

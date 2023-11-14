/*
* Created by boil on 2023/11/4.
*/
#if defined (__APPLE__)

#ifndef RENDU_COMMON_APPLE_SOCK_H
#define RENDU_COMMON_APPLE_SOCK_H

/* For POSIX systems we use the standard BSD socket API. */

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

// apple
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/uio.h>
#include <sys/filio.h>
#include <sys/event.h>

COMMON_NAMESPACE_BEGIN

#define SOCKET INT32

COMMON_NAMESPACE_END

#endif

#endif //RENDU_COMMON_APPLE_SOCK_H

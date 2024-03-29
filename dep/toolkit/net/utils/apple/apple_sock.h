/*
* Created by boil on 2023/11/4.
*/
#if defined (__APPLE__)

#ifndef RENDU_APPLE_SOCK_H
#define RENDU_APPLE_SOCK_H

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

RD_NAMESPACE_BEGIN

#define SOCKET int32

RD_NAMESPACE_END

#endif

#endif //RENDU_APPLE_SOCK_H

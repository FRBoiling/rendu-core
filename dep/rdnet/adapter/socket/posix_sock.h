/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_POSIX_SOCK_H
#define RENDU_POSIX_SOCK_H

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
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#endif

#endif //RENDU_POSIX_SOCK_H

/*
* Created by boil on 2023/10/22.
*/

#ifndef RENDU_NET_FWD_H
#define RENDU_NET_FWD_H

#define CONN_INFO_LEN   32
#define CONN_ADDR_STR_LEN 128 /* Similar to INET6_ADDRSTRLEN, hoping to handle other protocols. */
#define MAX_ACCEPTS_PER_CALL 1000


#define CONN_TYPE_SOCKET            "tcp"
#define CONN_TYPE_UNIX              "unix"
#define CONN_TYPE_TLS               "tls"
#define CONN_TYPE_MAX               8           /* 8 is enough to be extendable */


#define NET_IP_STR_LEN 46 /* INET6_ADDRSTRLEN is 46, but we need to be sure */

#define CONFIG_BINDADDR_MAX 16


#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

/* Flags used with certain functions. */
#define ANET_NONE 0
#define ANET_IP_ONLY (1<<0)

#if defined(__sun) || defined(_AIX)
#define AF_LOCAL AF_UNIX
#endif

#ifdef _AIX
#undef ip_len
#endif

#define EVENT_MASK_MALLOC_SIZE(sz) (((sz) + 3) / 4)
#define EVENT_MASK_OFFSET(fd) ((fd) % 4 * 2)
#define EVENT_MASK_ENCODE(fd, mask) (((mask) & 0x3) << EVENT_MASK_OFFSET(fd))


/* Error codes */
#define C_OK                    0
#define C_ERR                   -1

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0       /* No events registered. */
#define AE_READABLE 1   /* Fire when descriptor is readable. */
#define AE_WRITABLE 2   /* Fire when descriptor is writable. */
#define AE_BARRIER 4    /* With WRITABLE, never fire the event if the
                           READABLE event already fired in the same event
                           loop iteration. Useful when you want to persist
                           things to disk before sending replies, and want
                           to do that in a group fashion. */

#define AE_FILE_EVENTS (1<<0)
#define AE_TIME_EVENTS (1<<1)
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT (1<<2)
#define AE_CALL_BEFORE_SLEEP (1<<3)
#define AE_CALL_AFTER_SLEEP (1<<4)

#define AE_NOMORE -1
#define AE_DELETED_EVENT_ID -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

#define NET_HOST_STR_LEN 256 /* Longest valid hostname */
#define NET_IP_STR_LEN 46 /* INET6_ADDRSTRLEN is 46, but we need to be sure */
#define NET_ADDR_STR_LEN (NET_IP_STR_LEN+32) /* Must be enough for ip:port */
#define NET_HOST_PORT_STR_LEN (NET_HOST_STR_LEN+32) /* Must be enough for hostname:port */

#include "define.h"

RD_NAMESPACE_BEGIN


RD_NAMESPACE_END



#endif //RENDU_NET_FWD_H

/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_RD_ERRNO_H
#define RENDU_RD_ERRNO_H

#include <cerrno>

#define RD__EOF     (-4095)
#define RD__UNKNOWN (-4094)

#define RD__EAI_ADDRFAMILY  (-3000)
#define RD__EAI_AGAIN       (-3001)
#define RD__EAI_BADFLAGS    (-3002)
#define RD__EAI_CANCELED    (-3003)
#define RD__EAI_FAIL        (-3004)
#define RD__EAI_FAMILY      (-3005)
#define RD__EAI_MEMORY      (-3006)
#define RD__EAI_NODATA      (-3007)
#define RD__EAI_NONAME      (-3008)
#define RD__EAI_OVERFLOW    (-3009)
#define RD__EAI_SERVICE     (-3010)
#define RD__EAI_SOCKTYPE    (-3011)
#define RD__EAI_BADHINTS    (-3013)
#define RD__EAI_PROTOCOL    (-3014)

/* Only map to the system errno on non-Windows platforms. It's apparently
* a fairly common practice for Windows programmers to redefine errno codes.
*/
#if defined(E2BIG) && !defined(_WIN32)
# define RD__E2BIG (-E2BIG)
#else
# define RD__E2BIG (-4093)
#endif

#if defined(EACCES) && !defined(_WIN32)
# define RD__EACCES (-EACCES)
#else
# define RD__EACCES (-4092)
#endif

#if defined(EADDRINUSE) && !defined(_WIN32)
# define RD__EADDRINUSE (-EADDRINUSE)
#else
# define RD__EADDRINUSE (-4091)
#endif

#if defined(EADDRNOTAVAIL) && !defined(_WIN32)
# define RD__EADDRNOTAVAIL (-EADDRNOTAVAIL)
#else
# define RD__EADDRNOTAVAIL (-4090)
#endif

#if defined(EAFNOSUPPORT) && !defined(_WIN32)
# define RD__EAFNOSUPPORT (-EAFNOSUPPORT)
#else
# define RD__EAFNOSUPPORT (-4089)
#endif

#if defined(EAGAIN) && !defined(_WIN32)
# define RD__EAGAIN (-EAGAIN)
#else
# define RD__EAGAIN (-4088)
#endif

#if defined(EALREADY) && !defined(_WIN32)
# define RD__EALREADY (-EALREADY)
#else
# define RD__EALREADY (-4084)
#endif

#if defined(EBADF) && !defined(_WIN32)
# define RD__EBADF (-EBADF)
#else
# define RD__EBADF (-4083)
#endif

#if defined(EBUSY) && !defined(_WIN32)
# define RD__EBUSY (-EBUSY)
#else
# define RD__EBUSY (-4082)
#endif

#if defined(ECANCELED) && !defined(_WIN32)
# define RD__ECANCELED (-ECANCELED)
#else
# define RD__ECANCELED (-4081)
#endif

#if defined(ECHARSET) && !defined(_WIN32)
# define RD__ECHARSET (-ECHARSET)
#else
# define RD__ECHARSET (-4080)
#endif

#if defined(ECONNABORTED) && !defined(_WIN32)
# define RD__ECONNABORTED (-ECONNABORTED)
#else
# define RD__ECONNABORTED (-4079)
#endif

#if defined(ECONNREFUSED) && !defined(_WIN32)
# define RD__ECONNREFUSED (-ECONNREFUSED)
#else
# define RD__ECONNREFUSED (-4078)
#endif

#if defined(ECONNRESET) && !defined(_WIN32)
# define RD__ECONNRESET (-ECONNRESET)
#else
# define RD__ECONNRESET (-4077)
#endif

#if defined(EDESTADDRREQ) && !defined(_WIN32)
# define RD__EDESTADDRREQ (-EDESTADDRREQ)
#else
# define RD__EDESTADDRREQ (-4076)
#endif

#if defined(EEXIST) && !defined(_WIN32)
# define RD__EEXIST (-EEXIST)
#else
# define RD__EEXIST (-4075)
#endif

#if defined(EFAULT) && !defined(_WIN32)
# define RD__EFAULT (-EFAULT)
#else
# define RD__EFAULT (-4074)
#endif

#if defined(EHOSTUNREACH) && !defined(_WIN32)
# define RD__EHOSTUNREACH (-EHOSTUNREACH)
#else
# define RD__EHOSTUNREACH (-4073)
#endif

#if defined(EINTR) && !defined(_WIN32)
# define RD__EINTR (-EINTR)
#else
# define RD__EINTR (-4072)
#endif

#if defined(EINVAL) && !defined(_WIN32)
# define RD__EINVAL (-EINVAL)
#else
# define RD__EINVAL (-4071)
#endif

#if defined(EIO) && !defined(_WIN32)
# define RD__EIO (-EIO)
#else
# define RD__EIO (-4070)
#endif

#if defined(EISCONN) && !defined(_WIN32)
# define RD__EISCONN (-EISCONN)
#else
# define RD__EISCONN (-4069)
#endif

#if defined(EISDIR) && !defined(_WIN32)
# define RD__EISDIR (-EISDIR)
#else
# define RD__EISDIR (-4068)
#endif

#if defined(ELOOP) && !defined(_WIN32)
# define RD__ELOOP (-ELOOP)
#else
# define RD__ELOOP (-4067)
#endif

#if defined(EMFILE) && !defined(_WIN32)
# define RD__EMFILE (-EMFILE)
#else
# define RD__EMFILE (-4066)
#endif

#if defined(EMSGSIZE) && !defined(_WIN32)
# define RD__EMSGSIZE (-EMSGSIZE)
#else
# define RD__EMSGSIZE (-4065)
#endif

#if defined(ENAMETOOLONG) && !defined(_WIN32)
# define RD__ENAMETOOLONG (-ENAMETOOLONG)
#else
# define RD__ENAMETOOLONG (-4064)
#endif

#if defined(ENETDOWN) && !defined(_WIN32)
# define RD__ENETDOWN (-ENETDOWN)
#else
# define RD__ENETDOWN (-4063)
#endif

#if defined(ENETUNREACH) && !defined(_WIN32)
# define RD__ENETUNREACH (-ENETUNREACH)
#else
# define RD__ENETUNREACH (-4062)
#endif

#if defined(ENFILE) && !defined(_WIN32)
# define RD__ENFILE (-ENFILE)
#else
# define RD__ENFILE (-4061)
#endif

#if defined(ENOBUFS) && !defined(_WIN32)
# define RD__ENOBUFS (-ENOBUFS)
#else
# define RD__ENOBUFS (-4060)
#endif

#if defined(ENODEV) && !defined(_WIN32)
# define RD__ENODEV (-ENODEV)
#else
# define RD__ENODEV (-4059)
#endif

#if defined(ENOENT) && !defined(_WIN32)
# define RD__ENOENT (-ENOENT)
#else
# define RD__ENOENT (-4058)
#endif

#if defined(ENOMEM) && !defined(_WIN32)
# define RD__ENOMEM (-ENOMEM)
#else
# define RD__ENOMEM (-4057)
#endif

#if defined(ENONET) && !defined(_WIN32)
# define RD__ENONET (-ENONET)
#else
# define RD__ENONET (-4056)
#endif

#if defined(ENOSPC) && !defined(_WIN32)
# define RD__ENOSPC (-ENOSPC)
#else
# define RD__ENOSPC (-4055)
#endif

#if defined(ENOSYS) && !defined(_WIN32)
# define RD__ENOSYS (-ENOSYS)
#else
# define RD__ENOSYS (-4054)
#endif

#if defined(ENOTCONN) && !defined(_WIN32)
# define RD__ENOTCONN (-ENOTCONN)
#else
# define RD__ENOTCONN (-4053)
#endif

#if defined(ENOTDIR) && !defined(_WIN32)
# define RD__ENOTDIR (-ENOTDIR)
#else
# define RD__ENOTDIR (-4052)
#endif

#if defined(ENOTEMPTY) && !defined(_WIN32)
# define RD__ENOTEMPTY (-ENOTEMPTY)
#else
# define RD__ENOTEMPTY (-4051)
#endif

#if defined(ENOTSOCK) && !defined(_WIN32)
# define RD__ENOTSOCK (-ENOTSOCK)
#else
# define RD__ENOTSOCK (-4050)
#endif

#if defined(ENOTSUP) && !defined(_WIN32)
# define RD__ENOTSUP (-ENOTSUP)
#else
# define RD__ENOTSUP (-4049)
#endif

#if defined(EPERM) && !defined(_WIN32)
# define RD__EPERM (-EPERM)
#else
# define RD__EPERM (-4048)
#endif

#if defined(EPIPE) && !defined(_WIN32)
# define RD__EPIPE (-EPIPE)
#else
# define RD__EPIPE (-4047)
#endif

#if defined(EPROTO) && !defined(_WIN32)
# define RD__EPROTO (-EPROTO)
#else
# define RD__EPROTO (-4046)
#endif

#if defined(EPROTONOSUPPORT) && !defined(_WIN32)
# define RD__EPROTONOSUPPORT (-EPROTONOSUPPORT)
#else
# define RD__EPROTONOSUPPORT (-4045)
#endif

#if defined(EPROTOTYPE) && !defined(_WIN32)
# define RD__EPROTOTYPE (-EPROTOTYPE)
#else
# define RD__EPROTOTYPE (-4044)
#endif

#if defined(EROFS) && !defined(_WIN32)
# define RD__EROFS (-EROFS)
#else
# define RD__EROFS (-4043)
#endif

#if defined(ESHUTDOWN) && !defined(_WIN32)
# define RD__ESHUTDOWN (-ESHUTDOWN)
#else
# define RD__ESHUTDOWN (-4042)
#endif

#if defined(ESPIPE) && !defined(_WIN32)
# define RD__ESPIPE (-ESPIPE)
#else
# define RD__ESPIPE (-4041)
#endif

#if defined(ESRCH) && !defined(_WIN32)
# define RD__ESRCH (-ESRCH)
#else
# define RD__ESRCH (-4040)
#endif

#if defined(ETIMEDOUT) && !defined(_WIN32)
# define RD__ETIMEDOUT (-ETIMEDOUT)
#else
# define RD__ETIMEDOUT (-4039)
#endif

#if defined(ETXTBSY) && !defined(_WIN32)
# define RD__ETXTBSY (-ETXTBSY)
#else
# define RD__ETXTBSY (-4038)
#endif

#if defined(EXDEV) && !defined(_WIN32)
# define RD__EXDEV (-EXDEV)
#else
# define RD__EXDEV (-4037)
#endif

#if defined(EFBIG) && !defined(_WIN32)
# define RD__EFBIG (-EFBIG)
#else
# define RD__EFBIG (-4036)
#endif

#if defined(ENOPROTOOPT) && !defined(_WIN32)
# define RD__ENOPROTOOPT (-ENOPROTOOPT)
#else
# define RD__ENOPROTOOPT (-4035)
#endif

#if defined(ERANGE) && !defined(_WIN32)
# define RD__ERANGE (-ERANGE)
#else
# define RD__ERANGE (-4034)
#endif

#if defined(ENXIO) && !defined(_WIN32)
# define RD__ENXIO (-ENXIO)
#else
# define RD__ENXIO (-4033)
#endif

#if defined(EMLINK) && !defined(_WIN32)
# define RD__EMLINK (-EMLINK)
#else
# define RD__EMLINK (-4032)
#endif

/* EHOSTDOWN is not visible on BSD-like systems when _POSIX_C_SOURCE is
* defined. Fortunately, its value is always 64 so it's possible albeit
* icky to hard-code it.
*/
#if defined(EHOSTDOWN) && !defined(_WIN32)
# define RD__EHOSTDOWN (-EHOSTDOWN)
#elif defined(__APPLE__) || \
      defined(__DragonFly__) || \
      defined(__FreeBSD__) || \
      defined(__FreeBSD_kernel__) || \
      defined(__NetBSD__) || \
      defined(__OpenBSD__)
# define RD__EHOSTDOWN (-64)
#else
# define RD__EHOSTDOWN (-4031)
#endif

#if defined(EREMOTEIO) && !defined(_WIN32)
# define RD__EREMOTEIO (-EREMOTEIO)
#else
# define RD__EREMOTEIO (-4030)
#endif


#define RD_ERRNO_MAP(XX)                                                      \
  XX(E2BIG, "argument list too long")                                         \
  XX(EACCES, "permission denied")                                             \
  XX(EADDRINUSE, "GetAddress already in use")                                    \
  XX(EADDRNOTAVAIL, "GetAddress not available")                                  \
  XX(EAFNOSUPPORT, "GetAddress family not supported")                            \
  XX(EAGAIN, "resource temporarily unavailable")                              \
  XX(EAI_ADDRFAMILY, "GetAddress family not supported")                          \
  XX(EAI_AGAIN, "temporary failure")                                          \
  XX(EAI_BADFLAGS, "bad ai_flags value")                                      \
  XX(EAI_BADHINTS, "invalid value for hints")                                 \
  XX(EAI_CANCELED, "request canceled")                                        \
  XX(EAI_FAIL, "permanent failure")                                           \
  XX(EAI_FAMILY, "ai_family not supported")                                   \
  XX(EAI_MEMORY, "out of memory")                                             \
  XX(EAI_NODATA, "no GetAddress")                                                \
  XX(EAI_NONAME, "unknown node or service")                                   \
  XX(EAI_OVERFLOW, "argument buffer overflow")                                \
  XX(EAI_PROTOCOL, "resolved protocol is unknown")                            \
  XX(EAI_SERVICE, "service not available for socket type")                    \
  XX(EAI_SOCKTYPE, "socket type not supported")                               \
  XX(EALREADY, "connection already in progress")                              \
  XX(EBADF, "bad file descriptor")                                            \
  XX(EBUSY, "resource busy or locked")                                        \
  XX(ECANCELED, "operation canceled")                                         \
  XX(ECHARSET, "invalid Unicode character")                                   \
  XX(ECONNABORTED, "software caused connection abort")                        \
  XX(ECONNREFUSED, "connection refused")                                      \
  XX(ECONNRESET, "connection Reset by peer")                                  \
  XX(EDESTADDRREQ, "destination GetAddress required")                            \
  XX(EEXIST, "file already exists")                                           \
  XX(EFAULT, "bad GetAddress in system call argument")                           \
  XX(EFBIG, "file too large")                                                 \
  XX(EHOSTUNREACH, "host is unreachable")                                     \
  XX(EINTR, "interrupted system call")                                        \
  XX(EINVAL, "invalid argument")                                              \
  XX(EIO, "i/o error")                                                        \
  XX(EISCONN, "socket is already connected")                                  \
  XX(EISDIR, "illegal operation on a directory")                              \
  XX(ELOOP, "too many symbolic links encountered")                            \
  XX(EMFILE, "too many open files")                                           \
  XX(EMSGSIZE, "message too long")                                            \
  XX(ENAMETOOLONG, "name too long")                                           \
  XX(ENETDOWN, "network is down")                                             \
  XX(ENETUNREACH, "network is unreachable")                                   \
  XX(ENFILE, "file table overflow")                                           \
  XX(ENOBUFS, "no buffer space available")                                    \
  XX(ENODEV, "no such device")                                                \
  XX(ENOENT, "no such file or directory")                                     \
  XX(ENOMEM, "not enough memory")                                             \
  XX(ENONET, "machine is not on the network")                                 \
  XX(ENOPROTOOPT, "protocol not available")                                   \
  XX(ENOSPC, "no space left on device")                                       \
  XX(ENOSYS, "function not implemented")                                      \
  XX(ENOTCONN, "socket is not connected")                                     \
  XX(ENOTDIR, "not a directory")                                              \
  XX(ENOTEMPTY, "directory not IsEmpty")                                        \
  XX(ENOTSOCK, "socket operation on non-socket")                              \
  XX(ENOTSUP, "operation not supported on socket")                            \
  XX(EPERM, "operation not permitted")                                        \
  XX(EPIPE, "broken pipe")                                                    \
  XX(EPROTO, "protocol error")                                                \
  XX(EPROTONOSUPPORT, "protocol not supported")                               \
  XX(EPROTOTYPE, "protocol wrong type for socket")                            \
  XX(ERANGE, "result too large")                                              \
  XX(EROFS, "read-only file system")                                          \
  XX(ESHUTDOWN, "cannot send after transport endpoint shutdown")              \
  XX(ESPIPE, "invalid seek")                                                  \
  XX(ESRCH, "no such process")                                                \
  XX(ETIMEDOUT, "connection timed out")                                       \
  XX(ETXTBSY, "text file is busy")                                            \
  XX(EXDEV, "cross-device link not permitted")                                \
  XX(UNKNOWN, "unknown error")                                                \
  XX(EOF, "end of file")                                                      \
  XX(ENXIO, "no such device or GetAddress")                                      \
  XX(EMLINK, "too many links")                                                \
  XX(EHOSTDOWN, "host is down")                                               \
  XX(EREMOTEIO, "remote I/O error")                                           \


#include "common/define.h"

RD_NAMESPACE_BEGIN

  typedef enum {
#define XX(code, _) RD_ ## code = RD__ ## code,
    RD_ERRNO_MAP(XX)
#undef XX
    RD_ERRNO_MAX = RD__EOF - 1
  } rd_errno_t;

  const char *ErrorName(int err);

  const char *StrError(int err);

  int TranslatePosixError(int err);

//netErr参数在windows平台下才有效
  int GetError(bool netErr = true);

//netErr参数在windows平台下才有效
  const char *GetErrorMsg(bool netErr = true);


RD_NAMESPACE_END

#endif //RENDU_RD_ERRNO_H

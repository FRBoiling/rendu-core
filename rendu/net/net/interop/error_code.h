/*
* Created by boil on 2024/2/20.
*/

#ifndef RENDU_NET_NET_INTEROP_ERROR_CODE_H_
#define RENDU_NET_NET_INTEROP_ERROR_CODE_H_

#include "errno_define.h"

NET_NAMESPACE_BEGIN



namespace interop {

#define RD_ERRNO_MAP(XX)                                               \
  XX(FAIL, "Operation fail ")                                          \
  XX(SUCCESS, "Operation success ")                                    \
  XX(EPERM, "Operation not permitted ")                                \
  XX(ENOENT, "No such file or directory ")                             \
  XX(ESRCH, "No such process ")                                        \
  XX(EINTR, "Interrupted system call ")                                \
  XX(EIO, "Input/output error ")                                       \
  XX(ENXIO, "Device not configured ")                                  \
  XX(E2BIG, "Argument list too long ")                                 \
  XX(ENOEXEC, "Exec format error ")                                    \
  XX(EBADF, "Bad file descriptor ")                                    \
  XX(ECHILD, "No child processes ")                                    \
  XX(EDEADLK, "Resource deadlock avoided ")                            \
  XX(ENOMEM, "Cannot allocate memory ")                                \
  XX(EACCES, "Permission denied ")                                     \
  XX(EFAULT, "Bad address ")                                           \
  XX(ENOTBLK, "Block device required ")                                \
  XX(EBUSY, "Device / Resource busy ")                                 \
  XX(EEXIST, "File exists ")                                           \
  XX(EXDEV, "Cross-device link ")                                      \
  XX(ENODEV, "Operation not supported by device ")                     \
  XX(ENOTDIR, "Not a directory ")                                      \
  XX(EISDIR, "Is a directory ")                                        \
  XX(EINVAL, "Invalid argument ")                                      \
  XX(ENFILE, "Too many open files in system ")                         \
  XX(EMFILE, "Too many open files ")                                   \
  XX(ENOTTY, "Inappropriate ioctl for device ")                        \
  XX(ETXTBSY, "Text file busy ")                                       \
  XX(EFBIG, "File too large ")                                         \
  XX(ENOSPC, "No space left on device ")                               \
  XX(ESPIPE, "Illegal seek ")                                          \
  XX(EROFS, "Read-only file system ")                                  \
  XX(EMLINK, "Too many links ")                                        \
  XX(EPIPE, "Broken pipe ")                                            \
  XX(EDOM, "Numerical argument out of domain ")                        \
  XX(ERANGE, "Result too large ")                                      \
  XX(EAGAIN, "Resource temporarily unavailable ")                      \
  XX(EWOULDBLOCK, "Operation would block ")                            \
  XX(EINPROGRESS, "Operation now in progress ")                        \
  XX(EALREADY, "Operation already in progress ")                       \
  XX(ENOTSOCK, "Socket operation on non-socket ")                      \
  XX(EDESTADDRREQ, "Destination address required ")                    \
  XX(EMSGSIZE, "Message too long ")                                    \
  XX(EPROTOTYPE, "Protocol wrong type for socket ")                    \
  XX(ENOPROTOOPT, "Protocol not available ")                           \
  XX(EPROTONOSUPPORT, "Protocol not supported ")                       \
  XX(ESOCKTNOSUPPORT, "Socket type not supported ")                    \
  XX(ENOTSUP, "Operation not supported ")                              \
  XX(EOPNOTSUPP, "Operation not supported on socket ")                 \
  XX(EPFNOSUPPORT, "Protocol family not supported ")                   \
  XX(EAFNOSUPPORT, "Address family not supported by protocol family ") \
  XX(EADDRINUSE, "Address already in use ")                            \
  XX(EADDRNOTAVAIL, "Can't assign requested address ")                 \
  XX(ENETDOWN, "Network is down ")                                     \
  XX(ENETUNREACH, "Network is unreachable ")                           \
  XX(ENETRESET, "Network dropped connection on reset ")                \
  XX(ECONNABORTED, "Software caused connection abort ")                \
  XX(ECONNRESET, "Connection reset by peer ")                          \
  XX(ENOBUFS, "No buffer space available ")                            \
  XX(EISCONN, "Socket is already connected ")                          \
  XX(ENOTCONN, "Socket is not connected ")                             \
  XX(ESHUTDOWN, "Can't send after socket shutdown ")                   \
  XX(ETOOMANYREFS, "Too many references: can't splice ")               \
  XX(ETIMEDOUT, "Operation timed out ")                                \
  XX(ECONNREFUSED, "Connection refused ")                              \
  XX(ELOOP, "Too many levels of symbolic links ")                      \
  XX(ENAMETOOLONG, "File name too long ")                              \
  XX(EHOSTDOWN, "Host is down ")                                       \
  XX(EHOSTUNREACH, "No route to host ")                                \
  XX(ENOTEMPTY, "Directory not empty ")                                \
  XX(EPROCLIM, "Too many processes ")                                  \
  XX(EUSERS, "Too many users ")                                        \
  XX(EDQUOT, "Disc quota exceeded ")                                   \
  XX(ESTALE, "Stale NFS file handle ")                                 \
  XX(EREMOTE, "Too many levels of remote in path ")                    \
  XX(EBADRPC, "RPC struct is bad ")                                    \
  XX(ERPCMISMATCH, "RPC version wrong ")                               \
  XX(EPROGUNAVAIL, "RPC prog. not avail ")                             \
  XX(EPROGMISMATCH, "Program version wrong ")                          \
  XX(EPROCUNAVAIL, "Bad procedure for program ")                       \
  XX(ENOLCK, "No locks available ")                                    \
  XX(ENOSYS, "Function not implemented ")                              \
  XX(EFTYPE, "Inappropriate file type or format ")                     \
  XX(EAUTH, "Authentication error ")                                   \
  XX(ENEEDAUTH, "Need authenticator ")                                 \
  XX(EPWROFF, "Device power is off ")                                  \
  XX(EDEVERR, "Device error, e.g. paper out ")                         \
  XX(EOVERFLOW, "Value too large to be stored in data type ")          \
  XX(EBADEXEC, "Bad executable ")                                      \
  XX(EBADARCH, "Bad CPU type in executable ")                          \
  XX(ESHLIBVERS, "Shared library version mismatch ")                   \
  XX(EBADMACHO, "Malformed Macho file ")                               \
  XX(ECANCELED, "Operation canceled ")                                 \
  XX(EIDRM, "Identifier removed ")                                     \
  XX(ENOMSG, "No message of desired type ")                            \
  XX(EILSEQ, "Illegal byte sequence ")                                 \
  XX(ENOATTR, "Attribute not found ")                                  \
  XX(EBADMSG, "Bad message ")                                          \
  XX(EMULTIHOP, "Reserved ")                                           \
  XX(ENODATA, "No message available on STREAM ")                       \
  XX(ENOLINK, "Reserved ")                                             \
  XX(ENOSR, "No STREAM resources ")                                    \
  XX(ENOSTR, "Not a STREAM ")                                          \
  XX(EPROTO, "Protocol error ")                                        \
  XX(ETIME, "STREAM ioctl timeout ")                                   \
  XX(ENOPOLICY, "No such policy registered ")                          \
  XX(ENOTRECOVERABLE, "State not recoverable ")                        \
  XX(EOWNERDEAD, "Previous owner died ")                               \
  XX(EQFULL, "Interface output queue is full ")                        \
  XX(ELAST, "Must be equal largest errno ")
  //  XX(EHOSTNOTFOUND, "Host not find ")
  //  XX(ESOCKETERROR, "Socket error ")

  typedef enum {
#define XX(code, _) RD_##code = RD__##code,
    RD_ERRNO_MAP(XX)
#undef XX
  } Error;

  const string GetErrorName(int err);
  const string GetErrorMsg(int err);

  const Error ConvertToError(int raw_err);
  const int ConvertToRawError(Error err);

}// namespace interop

NET_NAMESPACE_END

#endif//RENDU_NET_NET_INTEROP_ERROR_CODE_H_

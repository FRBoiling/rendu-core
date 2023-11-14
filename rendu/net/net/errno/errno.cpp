/*
* Created by boil on 2023/10/26.
*/

#include "errno.h"
#include <cstdio>

#if defined(_WIN32)
#define FD_SETSIZE 1024 //修改默认64为1024路
#include <winsock2.h>
#include <windows.h>
#else
#include <cerrno>
#endif // defined(_WIN32)

COMMON_NAMESPACE_BEGIN

  static const char *rd__unknown_err_code(int err) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "Unknown system error %d", err);
    return buf;
  }

#define RD_ERR_NAME_GEN(name, _) case RD_ ## name: return #name;

  const char *rd_err_name(int err) {
    switch (err) {
      RD_ERRNO_MAP(RD_ERR_NAME_GEN)
    }
    return rd__unknown_err_code(err);
  }

#undef RD_ERR_NAME_GEN

#define RD_STRERROR_GEN(name, msg) case RD_ ## name: return msg;

  const char *StrError(int err) {
    switch (err) {
      RD_ERRNO_MAP(RD_STRERROR_GEN)
    }
    return rd__unknown_err_code(err);
  }

#undef RD_STRERROR_GEN

  int TranslatePosixError(int err) {
    if (err <= 0) {
      return err;
    }
    switch (err) {
      //为了兼容windows/unix平台，信号EINPROGRESS ，EAGAIN，EWOULDBLOCK，ENOBUFS 全部统一成EAGAIN处理
      case ENOBUFS://在mac系统下实测发现会有此信号发生
      case EINPROGRESS:
      case EWOULDBLOCK:
        err = EAGAIN;
        break;
      default:
        break;
    }
    return -err;
  }

  int GetError(bool netErr) {
#if defined(_WIN32)
    auto errCode = netErr ? WSAGetLastError() : GetLastError();
      switch (errCode) {
      case ERROR_NOACCESS:                    return RD_EACCES;
      case WSAEACCES:                         return RD_EACCES;
#if defined(ERROR_ELEVATION_REQUIRED)
      case ERROR_ELEVATION_REQUIRED:          return RD_EACCES;
#endif //ERROR_ELEVATION_REQUIRED
      case ERROR_ADDRESS_ALREADY_ASSOCIATED:  return RD_EADDRINUSE;
      case WSAEADDRINUSE:                     return RD_EADDRINUSE;
      case WSAEADDRNOTAVAIL:                  return RD_EADDRNOTAVAIL;
      case WSAEAFNOSUPPORT:                   return RD_EAFNOSUPPORT;
      case WSAEWOULDBLOCK:                    return RD_EAGAIN;
      case WSAEALREADY:                       return RD_EALREADY;
      case ERROR_INVALID_FLAGS:               return RD_EBADF;
      case ERROR_INVALID_HANDLE:              return RD_EBADF;
      case ERROR_LOCK_VIOLATION:              return RD_EBUSY;
      case ERROR_PIPE_BUSY:                   return RD_EBUSY;
      case ERROR_SHARING_VIOLATION:           return RD_EBUSY;
      case ERROR_OPERATION_ABORTED:           return RD_ECANCELED;
      case WSAEINTR:                          return RD_ECANCELED;
      case ERROR_NO_UNICODE_TRANSLATION:      return RD_ECHARSET;
      case ERROR_CONNECTION_ABORTED:          return RD_ECONNABORTED;
      case WSAECONNABORTED:                   return RD_ECONNABORTED;
      case ERROR_CONNECTION_REFUSED:          return RD_ECONNREFUSED;
      case WSAECONNREFUSED:                   return RD_ECONNREFUSED;
      case ERROR_NETNAME_DELETED:             return RD_ECONNRESET;
      case WSAECONNRESET:                     return RD_ECONNRESET;
      case ERROR_ALREADY_EXISTS:              return RD_EEXIST;
      case ERROR_FILE_EXISTS:                 return RD_EEXIST;
      case ERROR_BUFFER_OVERFLOW:             return RD_EFAULT;
      case WSAEFAULT:                         return RD_EFAULT;
      case ERROR_HOST_UNREACHABLE:            return RD_EHOSTUNREACH;
      case WSAEHOSTUNREACH:                   return RD_EHOSTUNREACH;
      case ERROR_INSUFFICIENT_BUFFER:         return RD_EINVAL;
      case ERROR_INVALID_DATA:                return RD_EINVAL;
      case ERROR_INVALID_PARAMETER:           return RD_EINVAL;
#if defined(ERROR_SYMLINK_NOT_SUPPORTED)
      case ERROR_SYMLINK_NOT_SUPPORTED:       return RD_EINVAL;
#endif //ERROR_SYMLINK_NOT_SUPPORTED
      case WSAEINVAL:                         return RD_EINVAL;
      case WSAEPFNOSUPPORT:                   return RD_EINVAL;
      case WSAESOCKTNOSUPPORT:                return RD_EINVAL;
      case ERROR_BEGINNING_OF_MEDIA:          return RD_EIO;
      case ERROR_BUS_RESET:                   return RD_EIO;
      case ERROR_CRC:                         return RD_EIO;
      case ERROR_DEVICE_DOOR_OPEN:            return RD_EIO;
      case ERROR_DEVICE_REQUIRES_CLEANING:    return RD_EIO;
      case ERROR_DISK_CORRUPT:                return RD_EIO;
      case ERROR_EOM_OVERFLOW:                return RD_EIO;
      case ERROR_FILEMARK_DETECTED:           return RD_EIO;
      case ERROR_GEN_FAILURE:                 return RD_EIO;
      case ERROR_INVALID_BLOCK_LENGTH:        return RD_EIO;
      case ERROR_IO_DEVICE:                   return RD_EIO;
      case ERROR_NO_DATA_DETECTED:            return RD_EIO;
      case ERROR_NO_SIGNAL_SENT:              return RD_EIO;
      case ERROR_OPEN_FAILED:                 return RD_EIO;
      case ERROR_SETMARK_DETECTED:            return RD_EIO;
      case ERROR_SIGNAL_REFUSED:              return RD_EIO;
      case WSAEISCONN:                        return RD_EISCONN;
      case ERROR_CANT_RESOLVE_FILENAME:       return RD_ELOOP;
      case ERROR_TOO_MANY_OPEN_FILES:         return RD_EMFILE;
      case WSAEMFILE:                         return RD_EMFILE;
      case WSAEMSGSIZE:                       return RD_EMSGSIZE;
      case ERROR_FILENAME_EXCED_RANGE:        return RD_ENAMETOOLONG;
      case ERROR_NETWORK_UNREACHABLE:         return RD_ENETUNREACH;
      case WSAENETUNREACH:                    return RD_ENETUNREACH;
      case WSAENOBUFS:                        return RD_ENOBUFS;
      case ERROR_BAD_PATHNAME:                return RD_ENOENT;
      case ERROR_DIRECTORY:                   return RD_ENOENT;
      case ERROR_FILE_NOT_FOUND:              return RD_ENOENT;
      case ERROR_INVALID_NAME:                return RD_ENOENT;
      case ERROR_INVALID_DRIVE:               return RD_ENOENT;
      case ERROR_INVALID_REPARSE_DATA:        return RD_ENOENT;
      case ERROR_MOD_NOT_FOUND:               return RD_ENOENT;
      case ERROR_PATH_NOT_FOUND:              return RD_ENOENT;
      case WSAHOST_NOT_FOUND:                 return RD_ENOENT;
      case WSANO_DATA:                        return RD_ENOENT;
      case ERROR_NOT_ENOUGH_MEMORY:           return RD_ENOMEM;
      case ERROR_OUTOFMEMORY:                 return RD_ENOMEM;
      case ERROR_CANNOT_MAKE:                 return RD_ENOSPC;
      case ERROR_DISK_FULL:                   return RD_ENOSPC;
      case ERROR_EA_TABLE_FULL:               return RD_ENOSPC;
      case ERROR_END_OF_MEDIA:                return RD_ENOSPC;
      case ERROR_HANDLE_DISK_FULL:            return RD_ENOSPC;
      case ERROR_NOT_CONNECTED:               return RD_ENOTCONN;
      case WSAENOTCONN:                       return RD_ENOTCONN;
      case ERROR_DIR_NOT_EMPTY:               return RD_ENOTEMPTY;
      case WSAENOTSOCK:                       return RD_ENOTSOCK;
      case ERROR_NOT_SUPPORTED:               return RD_ENOTSUP;
      case ERROR_BROKEN_PIPE:                 return RD_EOF;
      case ERROR_ACCESS_DENIED:               return RD_EPERM;
      case ERROR_PRIVILEGE_NOT_HELD:          return RD_EPERM;
      case ERROR_BAD_PIPE:                    return RD_EPIPE;
      case ERROR_NO_DATA:                     return RD_EPIPE;
      case ERROR_PIPE_NOT_CONNECTED:          return RD_EPIPE;
      case WSAESHUTDOWN:                      return RD_EPIPE;
      case WSAEPROTONOSUPPORT:                return RD_EPROTONOSUPPORT;
      case ERROR_WRITE_PROTECT:               return RD_EROFS;
      case ERROR_SEM_TIMEOUT:                 return RD_ETIMEDOUT;
      case WSAETIMEDOUT:                      return RD_ETIMEDOUT;
      case ERROR_NOT_SAME_DEVICE:             return RD_EXDEV;
      case ERROR_INVALID_FUNCTION:            return RD_EISDIR;
      case ERROR_META_EXPANSION_TOO_LONG:     return RD_E2BIG;
      default:                                return errCode;
      }
#else
    return TranslatePosixError(errno);
#endif // defined(_WIN32)
  }

  const char *GetErrorMsg(bool netErr) {
    return StrError(GetError(netErr));
  }

COMMON_NAMESPACE_END
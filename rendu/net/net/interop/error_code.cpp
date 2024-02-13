/*
* Created by boil on 2024/2/20.
*/

#include "error_code.h"

#if defined(_WIN32)
#define FD_SETSIZE 1024//修改默认64为1024路
#include <windows.h>
#include <winsock2.h>
#else
#include <cerrno>
#endif// defined(_WIN32)

NET_NAMESPACE_BEGIN

namespace interop {

  static string UnknownErrorMsg(int err) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "Unknown system error %d", err);
    return buf;
  }

#define RD_ERR_NAME_GEN(name, _) \
  case RD_##name:                \
    return #name;

  const string GetErrorName(int err) {
    switch (err) {
      RD_ERRNO_MAP(RD_ERR_NAME_GEN)
    }

    return UnknownErrorMsg(err);
  }

#undef RD_ERR_NAME_GEN

#define RD_ERR_MSG_GEN(name, msg) \
  case RD_##name:                 \
    return msg;

  const string GetErrorMsg(int err) {
    switch (err) {
      RD_ERRNO_MAP(RD_ERR_MSG_GEN)
    }
    return UnknownErrorMsg(err);
  }

#undef RD_ERR_MSG_GEN

#define RD_ERR_CASE_GEN(name, _) \
  case Error::RD_##name:         \
    return Error::RD_##name;

  const Error ConvertToError(int raw_err) {
    switch (raw_err) {
      RD_ERRNO_MAP(RD_ERR_CASE_GEN);
    }
    return Error::RD_FAIL;
  }

#undef RD_ERR_CASE_GEN

#define RD_RAW_ERR_CASE_GEN(name, _) \
  case Error::RD_##name:             \
    return name;

  const int ConvertToRawError(Error err) {
    switch (err) {
        RD_ERRNO_MAP(RD_RAW_ERR_CASE_GEN);
    }
    return (int) err;
  }

#undef RD_RAW_ERR_CASE_GEN

}// namespace interop

NET_NAMESPACE_END
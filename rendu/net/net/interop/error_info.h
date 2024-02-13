/*
* Created by boil on 2024/2/20.
*/

#ifndef RENDU_NET_NET_INTEROP_ERROR_INFO_H_
#define RENDU_NET_NET_INTEROP_ERROR_INFO_H_

#include "net_define.h"
#include "error_code.h"

NET_NAMESPACE_BEGIN

namespace interop {

  struct ErrorInfo {
  private:
    Error m_error;
    int m_raw_errno;

  public:
    ErrorInfo(Error native_err) {
      m_error = native_err;
      m_raw_errno = ConvertToRawError(m_error);
    }

    Error GetError() { return m_error; }
    Error SetError(Error error) {
      m_error = error;
      return m_error;
    }

    int GetRawErrno();

    string GetErrorMessage();

    string ToString();
  };

}// namespace interop

NET_NAMESPACE_END

#endif//RENDU_NET_NET_INTEROP_ERROR_INFO_H_

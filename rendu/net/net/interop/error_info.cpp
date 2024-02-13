/*
* Created by boil on 2024/2/20.
*/

#include "error_info.h"

#include "sys.h"

NET_NAMESPACE_BEGIN

namespace interop {

  int ErrorInfo::GetRawErrno() {
    return m_raw_errno != -1 ? m_raw_errno : (m_raw_errno = interop::ConvertToRawError(m_error));
  }

  string ErrorInfo::GetErrorMessage(){
    return interop::GetErrorMsg(m_error);
  }

  string ToString(){
//    DefaultInterpolatedStringHandler interpolatedStringHandler = new DefaultInterpolatedStringHandler(36, 3);
//    interpolatedStringHandler.AppendLiteral("RawErrno: ");
//    interpolatedStringHandler.AppendFormatted<int>(this.RawErrno);
//    interpolatedStringHandler.AppendLiteral(" Error: ");
//    interpolatedStringHandler.AppendFormatted<Interop.Error>(this.Error);
//    interpolatedStringHandler.AppendLiteral(" GetErrorMessage: ");
//    interpolatedStringHandler.AppendFormatted(this.GetErrorMessage());
//    return interpolatedStringHandler.ToStringAndClear();
    return "";
  }


}// namespace interop

NET_NAMESPACE_END

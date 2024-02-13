/*
* Created by boil on 2024/2/18.
*/

#include "safe_handle.h"
#include "sys.h"

NET_NAMESPACE_BEGIN

namespace interop {

  int SafeHandle::Close() {
    if (m_fd != -1){
      int ret = interop::Sys::Close(m_fd);
      if (ret == -1){
        return ret;
      }
      m_fd = -1;
    }
    return 0;
  }

}// namespace interop

NET_NAMESPACE_END

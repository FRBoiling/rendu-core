/*
* Created by boil on 2023/11/2.
*/

#include "str_error.h"
#include <string>

RD_NAMESPACE_BEGIN

  __thread char t_errnobuf[512];

  int strerror_tl(int savedErrno) {
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
  }


RD_NAMESPACE_END


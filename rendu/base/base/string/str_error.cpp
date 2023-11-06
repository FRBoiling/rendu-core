/*
* Created by boil on 2023/11/2.
*/

#include "str_error.h"
#include <string>

COMMON_NAMESPACE_BEGIN

  __thread char t_errnobuf[512];

  int strerror_tl(int savedErrno) {
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
  }

<<<<<<<< HEAD:rendu/common/common/utils/str_error.cpp

COMMON_NAMESPACE_END
========
RD_NAMESPACE_END
>>>>>>>> f6e2ef7 (✨ feat(框架): 整理项目结构):rendu/base/base/string/str_error.cpp

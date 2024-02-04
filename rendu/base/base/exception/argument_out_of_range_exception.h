/*
* Created by boil on 2024/2/5.
*/

#ifndef RENDU_BASE_BASE_EXCEPTION_ARGUMENT_OUT_OF_RANGE_EXCEPTION_H_
#define RENDU_BASE_BASE_EXCEPTION_ARGUMENT_OUT_OF_RANGE_EXCEPTION_H_

#include "exception.h"

RD_NAMESPACE_BEGIN

class ArgumentOutOfRangeException : public Exception {
public:
  template<typename... Args>
  ArgumentOutOfRangeException(string_view_t fmt, Args &&...args): Exception(fmt,args...) {
  }
public:
};

RD_NAMESPACE_END

#endif//RENDU_BASE_BASE_EXCEPTION_ARGUMENT_OUT_OF_RANGE_EXCEPTION_H_

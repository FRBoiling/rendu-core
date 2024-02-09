/*
* Created by boil on 2024/2/9.
*/

#ifndef RENDU_BASE_BASE_EXCEPTION_ARGUMENT_NULL_EXCEPTION_H_
#define RENDU_BASE_BASE_EXCEPTION_ARGUMENT_NULL_EXCEPTION_H_

#include "exception.h"

RD_NAMESPACE_BEGIN

class ArgumentNullException : public Exception {
public:
  template<typename... Args>
  ArgumentNullException(string_view_t fmt, Args &&...args): Exception(fmt,args...) {
  }
};

RD_NAMESPACE_END

#endif//RENDU_BASE_BASE_EXCEPTION_ARGUMENT_NULL_EXCEPTION_H_

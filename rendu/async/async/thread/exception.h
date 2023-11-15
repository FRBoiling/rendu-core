/*
* Created by boil on 2023/11/13.
*/

#ifndef RENDU_ASYNC_EXCEPTION_H
#define RENDU_ASYNC_EXCEPTION_H

#include "async_define.h"

ASYNC_NAMESPACE_BEGIN

class CException : public std::exception {
public:
  CException(std::string what);

  ~CException() noexcept override = default;

  // default copy-ctor and operator= are okay.

  const char *what() const noexcept override {
    return message_.c_str();
  }

  const char *stackTrace() const noexcept {
    return stack_.c_str();
  }

private:
  std::string message_;
  std::string stack_;
};

ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_EXCEPTION_H

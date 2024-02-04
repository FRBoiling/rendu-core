/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_EXCEPTION_H
#define RENDU_EXCEPTION_H

#include "base_define.h"
#include "string/string_helper.h"
#include <exception>

RD_NAMESPACE_BEGIN

class Exception : public std::exception {
 public:
  template<typename... Args>
  Exception(string_view_t fmt, Args &&...args) {
  }

  ~Exception() noexcept override = default;

  // default copy-ctor and operator= are okay.

  const char *what() const noexcept override {
    return message_.c_str();
  }

  const char *StackTrace() const noexcept {
    return stack_.c_str();
  }

 private:
  STRING message_;
  STRING stack_;
};

RD_NAMESPACE_END

#endif//RENDU_EXCEPTION_H

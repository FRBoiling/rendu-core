/*
* Created by boil on 2024/1/28.
*/

#ifndef RENDU_EXCEPTION_H
#define RENDU_EXCEPTION_H

#include "base_define.h"
#include "string/string_helper.h"
#include <exception>

RD_NAMESPACE_BEGIN

class Exception : std::exception {
public:
  template<typename... Args>
  Exception(Args &&...args)
  {
  }

  template<typename... Args>
  Exception(string_view_t fmt, Args &&...args) {
  }

};

RD_NAMESPACE_END

#endif//RENDU_EXCEPTION_H

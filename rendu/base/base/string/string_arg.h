/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_STRING_ARG_H
#define RENDU_COMMON_STRING_ARG_H

#include "base_define.h"

RD_NAMESPACE_BEGIN

  class StringArg // copyable
  {
  public:
    explicit StringArg(const char *str)
      : str_(str) {}

    explicit StringArg(const string &str)
      : str_(str.c_str()) {}

    [[nodiscard]] const char *c_str() const { return str_; }

  private:
    const char *str_;
  };

RD_NAMESPACE_END

#endif //RENDU_COMMON_STRING_ARG_H

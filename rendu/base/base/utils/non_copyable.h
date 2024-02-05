/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_BASE_NON_COPYABLE_H
#define RENDU_BASE_NON_COPYABLE_H

#include "base_define.h"

RD_NAMESPACE_BEGIN

class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;

  void operator=(const NonCopyable &) = delete;

protected:
  NonCopyable() = default;

  ~NonCopyable() = default;
};

RD_NAMESPACE_END

#endif//RENDU_BASE_NON_COPYABLE_H

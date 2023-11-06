/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_NON_COPYABLE_H
#define RENDU_COMMON_NON_COPYABLE_H

#include "define.h"

COMMON_NAMESPACE_BEGIN

  class NonCopyable {
  public:
    NonCopyable(const NonCopyable &) = delete;

    void operator=(const NonCopyable &) = delete;

  protected:
    NonCopyable() = default;

    ~NonCopyable() = default;
  };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_NON_COPYABLE_H

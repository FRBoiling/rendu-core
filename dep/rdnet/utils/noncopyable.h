/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_NONCOPYABLE_H
#define RENDU_NONCOPYABLE_H

#include "define.h"

RD_NAMESPACE_BEGIN

  class noncopyable {
  public:
    noncopyable(const noncopyable &) = delete;

    void operator=(const noncopyable &) = delete;

  protected:
    noncopyable() = default;

    ~noncopyable() = default;
  };

RD_NAMESPACE_END

#endif //RENDU_NONCOPYABLE_H

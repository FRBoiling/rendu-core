/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_COMMON_COPYABLE_H
#define RENDU_COMMON_COPYABLE_H

#include "define.h"

COMMON_NAMESPACE_BEGIN

  class Copyable {
  protected:
    Copyable() = default;

    ~Copyable() = default;

  };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_COPYABLE_H

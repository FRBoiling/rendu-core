/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_BASE_COPYABLE_H
#define RENDU_BASE_COPYABLE_H

#include "base_define.h"

RD_NAMESPACE_BEGIN

  class Copyable {
  protected:
    Copyable() = default;

    ~Copyable() = default;

  };

RD_NAMESPACE_END

#endif //RENDU_BASE_COPYABLE_H

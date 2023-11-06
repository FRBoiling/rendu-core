/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_COMMON_OUT_OF_RANGE_EXCEPTION_H
#define RENDU_COMMON_OUT_OF_RANGE_EXCEPTION_H

#include "common/define.h"

COMMON_NAMESPACE_BEGIN

class OutOfRangeException :std::exception{

public:
  OutOfRangeException() {

  }
};

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_OUT_OF_RANGE_EXCEPTION_H

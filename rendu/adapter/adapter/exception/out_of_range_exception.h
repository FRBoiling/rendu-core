/*
* Created by boil on 2023/9/29.
*/

#ifndef RENDU_OUT_OF_RANGE_EXCEPTION_H
#define RENDU_OUT_OF_RANGE_EXCEPTION_H

#include "define.h"

RD_NAMESPACE_BEGIN

class OutOfRangeException :std::exception{

public:
  OutOfRangeException() {

  }
};

RD_NAMESPACE_END

#endif //RENDU_OUT_OF_RANGE_EXCEPTION_H

/*
* Created by boil on 2023/11/12.
*/

#ifndef RENDU_BASE_EXCEPTION_HELPER_H
#define RENDU_BASE_EXCEPTION_HELPER_H

#include "argument_null_exception.h"
#include "argument_out_of_range_exception.h"
#include "base_define.h"
#include "exception.h"
#include "exception_argument.h"
#include "out_of_range_exception.h"
#include "exception_resoure.h"

RD_NAMESPACE_BEGIN

class ThrowHelper {
public:
  static void ThrowArgumentNullException(ExceptionArgument argument) {
    throw ArgumentNullException("ThrowArgumentNullException");
  }

  static void ThrowArgumentOutOfRangeException(ExceptionArgument argument, ExceptionResource resource){
    throw ArgumentOutOfRangeException("ThrowArgumentOutOfRangeException");
  }
};

RD_NAMESPACE_END

#endif//RENDU_BASE_EXCEPTION_HELPER_H

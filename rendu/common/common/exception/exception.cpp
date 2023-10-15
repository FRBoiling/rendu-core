/*
* Created by boil on 2023/11/2.
*/

#include "exception.h"
#include "thread/current_thread.h"

RD_NAMESPACE_BEGIN

  Exception::Exception(string msg)
    : message_(std::move(msg)),
      stack_(CurrentThread::stackTrace(/*demangle=*/false))
  {
  }

RD_NAMESPACE_END
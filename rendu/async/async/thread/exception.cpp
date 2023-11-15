/*
* Created by boil on 2023/11/13.
*/

#include "exception.h"
#include "current_thread.h"

ASYNC_NAMESPACE_BEGIN

CException::CException(std::string msg)
    : message_(std::move(msg)),
      stack_(CurrentThread::stackTrace(/*demangle=*/false)) {
}

ASYNC_NAMESPACE_END

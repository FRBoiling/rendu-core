/*
* Created by boil on 2023/11/13.
*/

#include "exception.h"
#include "thread/current_thread.h"

COMMON_NAMESPACE_BEGIN

    Exception::Exception(STRING msg)
      : message_(std::move(msg)),
        stack_(CurrentThread::stackTrace(/*demangle=*/false)) {
    }

COMMON_NAMESPACE_END

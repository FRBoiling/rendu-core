/*
* Created by boil on 2023/10/26.
*/

#include "task_executor.h"
#include "task_executor_getter.h"

RD_NAMESPACE_BEGIN

  TaskExecutor::TaskExecutor(uint64_t max_size, uint64_t max_usec) : ThreadLoadCounter(max_size, max_usec) {}

RD_NAMESPACE_END
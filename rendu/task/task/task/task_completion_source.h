/*
* Created by boil on 2024/2/10.
*/

#ifndef RENDU_TASK_TASK_TASK_TASK_COMPLETION_SOURCE_H_
#define RENDU_TASK_TASK_TASK_TASK_COMPLETION_SOURCE_H_

#include "task_define.h"
#include "task.hpp"

TASK_NAMESPACE_BEGIN

template<class TResult>
class TaskCompletionSource
{
private:
  Task<TResult>* _task;

public:
  TaskCompletionSource() : _task(nullptr) {}

  void SetException(std::exception_ptr exception)
  {
    if (!TrySetException(exception))
    {
      throw std::logic_error("Task has already been completed.");
    }
  }

  bool TrySetException(std::exception_ptr exception)
  {
    if (!exception)
    {
      throw std::invalid_argument("Exception argument is null.");
    }

    if (_task->IsReady())
    {
      _task->GetPromise().SetException(exception);
      return true;
    }
    else
    {
      return false;
    }
  }

  void SetResult(TResult result)
  {
    if (!TrySetResult(result))
    {
      throw std::logic_error("Task has already been completed.");
    }
  }

  bool TrySetResult(TResult result)
  {
    if (_task->IsReady())
    {
      _task->GetPromise().SetValue(result);
      return true;
    }
    else
    {
      return false;
    }
  }
};

TASK_NAMESPACE_END

#endif//RENDU_TASK_TASK_TASK_TASK_COMPLETION_SOURCE_H_

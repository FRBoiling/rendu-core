/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_I_TASK_CANCELABLE_H
#define RENDU_I_TASK_CANCELABLE_H

#include "utils/noncopyable.h"

RD_NAMESPACE_BEGIN

class ITaskCancelable : public noncopyable {
public:
  ITaskCancelable() = default;

  virtual ~ITaskCancelable() = default;

  virtual void Cancel() = 0;
};

RD_NAMESPACE_END

#endif //RENDU_I_TASK_CANCELABLE_H

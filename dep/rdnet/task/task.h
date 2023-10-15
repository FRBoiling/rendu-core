/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_TASK_H
#define RENDU_TASK_H

#include "task_cancelable.h"

RD_NAMESPACE_BEGIN

  using TaskIn = std::function<void()>;
  using Task = TaskCancelable<void()>;

RD_NAMESPACE_END

#endif //RENDU_TASK_H

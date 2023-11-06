/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_I_TASK_EXECUTOR_GETTER_H
#define RENDU_I_TASK_EXECUTOR_GETTER_H

#include "task_executor.h"

RD_NAMESPACE_BEGIN

class ITaskExecutorGetter {
public:
  using Ptr = std::shared_ptr<ITaskExecutorGetter>;

  virtual ~ITaskExecutorGetter() = default;
  /**
   * 获取任务执行器
   * @return 任务执行器
   */
  virtual TaskExecutor::Ptr GetExecutor() = 0;

  /**
   * 获取执行器个数
   */
  virtual size_t GetExecutorSize() const = 0;
};

RD_NAMESPACE_END

#endif //RENDU_I_TASK_EXECUTOR_GETTER_H

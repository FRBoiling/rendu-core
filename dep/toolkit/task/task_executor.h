/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_TASK_EXECUTOR_H
#define RENDU_TASK_EXECUTOR_H

#include "a_task_executor.h"
#include "thread/thread_load_counter.h"

RD_NAMESPACE_BEGIN
/**
* 任务执行器
*/
class TaskExecutor : public ThreadLoadCounter, public ATaskExecutor {
public:
  using Ptr = std::shared_ptr<TaskExecutor>;

  /**
   * 构造函数
   * @param max_size cpu负载统计样本数
   * @param max_usec cpu负载统计时间窗口大小
   */
  explicit TaskExecutor(uint64_t max_size = 32, uint64_t max_usec = 2 * 1000 * 1000);
  ~TaskExecutor() override = default;
};

RD_NAMESPACE_END

#endif //RENDU_TASK_EXECUTOR_H

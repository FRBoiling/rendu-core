/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_A_TASK_EXECUTOR_H
#define RENDU_A_TASK_EXECUTOR_H

#include "task.h"

RD_NAMESPACE_BEGIN

  class ATaskExecutor {
  public:
    ATaskExecutor() = default;
    virtual ~ATaskExecutor() = default;

    /**
     * 异步执行任务
     * @param task 任务
     * @param may_sync 是否允许同步执行该任务
     * @return 任务是否添加成功
     */
    virtual Task::Ptr Async(TaskIn task, bool may_sync = true) = 0;

    /**
     * 最高优先级方式异步执行任务
     * @param task 任务
     * @param may_sync 是否允许同步执行该任务
     * @return 任务是否添加成功
     */
    virtual Task::Ptr AsyncFirst(TaskIn task, bool may_sync = true);

    /**
     * 同步执行任务
     * @param task
     * @return
     */
    void Sync(const TaskIn &task);

    /**
     * 最高优先级方式同步执行任务
     * @param task
     * @return
     */
    void SyncFirst(const TaskIn &task);
  };

RD_NAMESPACE_END

#endif //RENDU_A_TASK_EXECUTOR_H

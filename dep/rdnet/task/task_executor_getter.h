/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_TASK_EXECUTOR_GETTER_H
#define RENDU_TASK_EXECUTOR_GETTER_H

#include "i_task_executor_getter.h"
#include "task_executor.h"

RD_NAMESPACE_BEGIN

  class TaskExecutorGetter : public ITaskExecutorGetter {
  public:
    TaskExecutorGetter() : _thread_pos(0) {};

    ~TaskExecutorGetter() override = default;

  public:
    /**
     * 根据线程负载情况，获取最空闲的任务执行器
     * @return 任务执行器
     */
    TaskExecutor::Ptr GetExecutor() override;

    /**
      * 获取线程数
      */
    [[nodiscard]] size_t GetExecutorSize() const override;

  public:
    /**
     * 获取所有线程的负载率
     * @return 所有线程的负载率
     */
    std::vector<int32> GetExecutorLoadVec();

    /**
     * 获取所有线程任务执行延时，单位毫秒
     * 通过此函数也可以大概知道线程负载情况
     * @return
     */
    void GetExecutorDelay(const std::function<void(const std::vector<int32> &)> &callback);

    /**
     * 遍历所有线程
     */
    void for_each(const std::function<void(const TaskExecutor::Ptr &)> &cb);


  protected:
    size_t addPoller(const std::string &name, size_t size, int priority, bool register_thread,
                     bool enable_cpu_affinity = true);

  protected:
    size_t _thread_pos;
    std::vector<TaskExecutor::Ptr> _threads;
  };

RD_NAMESPACE_END

#endif //RENDU_TASK_EXECUTOR_GETTER_H

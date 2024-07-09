/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_ASYNC_ASYNC_THREAD_THREAD_SECONDARY_H_
#define RENDU_ASYNC_ASYNC_THREAD_THREAD_SECONDARY_H_

#include "thread_base.h"

RD_ASYNC_NAMESPACE_BEGIN

class ThreadSecondary : public ThreadBase {
public:
  explicit ThreadSecondary() {
    cur_ttl_ = 0;
    type_ = RD_THREAD_TYPE_SECONDARY;
  }
  

protected:
  Status init() override {
    RD_FUNCTION_BEGIN
    RD_ASSERT_INIT(false)
    RD_ASSERT_NOT_NULL(config_)

    cur_ttl_ = config_->secondary_thread_ttl_;
    is_init_ = true;
    thread_ = std::move(std::thread(&ThreadSecondary::run, this));
    setSchedParam();
    RD_FUNCTION_END
  }


  /**
     * 设置pool的信息
     * @param poolTaskQueue
     * @param poolPriorityTaskQueue
     * @param config
     * @return
     */
  Status setThreadPoolInfo(AtomicQueue<Task>* poolTaskQueue,
                            AtomicPriorityQueue<Task>* poolPriorityTaskQueue,
                            ThreadPoolConfigPtr config) {
    RD_FUNCTION_BEGIN
    RD_ASSERT_INIT(false)    // 初始化之前，设置参数
    RD_ASSERT_NOT_NULL(poolTaskQueue, poolPriorityTaskQueue, config)

    this->pool_task_queue_ = poolTaskQueue;
    this->pool_priority_task_queue_ = poolPriorityTaskQueue;
    this->config_ = config;
    RD_FUNCTION_END
  }


  Status run() final {
    RD_FUNCTION_BEGIN
    RD_ASSERT_INIT(true)

    status = loopProcess();
    RD_FUNCTION_END
  }


  Void processTask() override {
    Task task;
    if (popPoolTask(task)) {
      runTask(task);
    } else {
      // 如果单词无法获取，则稍加等待
      waitRunTask(config_->queue_emtpy_interval_);
    }
  }


  Void processTasks() override {
    TaskArr tasks;
    if (popPoolTask(tasks)) {
      runTasks(tasks);
    } else {
      waitRunTask(config_->queue_emtpy_interval_);
    }
  }


  /**
     * 有等待的执行任务
     * @param ms
     * @return
     * @notice 目的是降低cpu的占用率
     */
  Void waitRunTask(MSec ms) {
    auto task = this->pool_task_queue_->popWithTimeout(ms);
    if (nullptr != task) {
      (*task)();
    }
  }


  /**
     * 判断本线程是否需要被自动释放
     * @return
     */
  bool freeze() {
    if (likely(is_running_)) {
      cur_ttl_++;
      cur_ttl_ = std::min(cur_ttl_, config_->secondary_thread_ttl_);
    } else {
      cur_ttl_--;    // 如果当前线程没有在执行，则ttl-1
    }

    return cur_ttl_ <= 0 && done_;    // 必须是正在执行的线程，才可以被回收
  }

private:
  int cur_ttl_ = 0;                                                      // 当前最大生存周期

  friend class ThreadPool;
};

using ThreadSecondaryPtr = ThreadSecondary *;
  
RD_ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_THREAD_THREAD_SECONDARY_H_

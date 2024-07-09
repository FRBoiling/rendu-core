/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_ASYNC_THREAD_THREAD_BASE_H_
#define RENDU_ASYNC_THREAD_THREAD_BASE_H_

#include "assert/assert_define.h"
#include "queue/queue_include.h"
#include "task/task_include.h"
#include "thread_object.h"
#include "thread_pool/thread_pool_config.h"

RD_ASYNC_NAMESPACE_BEGIN

class ThreadBase : public ThreadObject {
protected:
  explicit ThreadBase() {
    done_ = true;
    is_init_ = false;
    is_running_ = false;
    pool_task_queue_ = nullptr;
    pool_priority_task_queue_ = nullptr;
    config_ = nullptr;
    total_task_num_ = 0;
  }


  ~ThreadBase() {
    reset();
  }


  /**
     * 所有线程类的 destroy 函数应该是一样的
     * 但是init函数不一样，因为线程构造函数不同
     * @return
     */
  Status destroy() override {
    RD_FUNCTION_BEGIN
    RD_ASSERT_INIT(true)

    reset();
    RD_FUNCTION_END
  }


  /**
     * 从线程池的队列中，获取任务
     * @param task
     * @return
     */
  virtual bool popPoolTask(TaskRef task) {
    bool result = pool_task_queue_->tryPop(task);
    if (!result && RD_THREAD_TYPE_SECONDARY == type_) {
      // 如果辅助线程没有获取到的话，还需要再尝试从长时间任务队列中，获取一次
      result = pool_priority_task_queue_->tryPop(task);
    }
    return result;
  }


  /**
     * 从线程池的队列中中，获取批量任务
     * @param tasks
     * @return
     */
  virtual bool popPoolTask(TaskArrRef tasks) {
    bool result = pool_task_queue_->tryPop(tasks, config_->max_pool_batch_size_);
    if (!result && RD_THREAD_TYPE_SECONDARY == type_) {
      result = pool_priority_task_queue_->tryPop(tasks, 1);// 从优先队列里，最多pop出来一个
    }
    return result;
  }


  /**
     * 执行单个任务
     * @param task
     */
  Void runTask(Task &task) {
    is_running_ = true;
    task();
    total_task_num_++;
    is_running_ = false;
  }


  /**
     * 批量执行任务
     * @param tasks
     */
  Void runTasks(TaskArr &tasks) {
    is_running_ = true;
    for (auto &task: tasks) {
      task();
    }
    total_task_num_ += tasks.size();
    is_running_ = false;
  }


  /**
     * 清空所有任务内容
     */
  Void reset() {
    done_ = false;
    if (thread_.joinable()) {
      thread_.join();// 等待线程结束
    }
    is_init_ = false;
    is_running_ = false;
    total_task_num_ = 0;
  }

  /**
     * 执行单个消息
     * @return
     */
  virtual Void processTask() = 0;


  /**
     * 获取批量执行task信息
     */
  virtual Void processTasks() = 0;


  /**
     * 循环处理任务
     * @return
     */
  Status loopProcess() {
    RD_FUNCTION_BEGIN
    RD_ASSERT_NOT_NULL(config_)

    if (config_->batch_task_enable_) {
      while (done_) {
        processTasks();// 批量任务获取执行接口
      }
    } else {
      while (done_) {
        processTask();// 单个任务获取执行接口
      }
    }

    RD_FUNCTION_END
  }


  /**
    * 设置线程优先级，仅针对非windows平台使用
    */
  Void setSchedParam() {
#ifndef _WIN32
    int priority = RD_THREAD_SCHED_OTHER;
    int policy = RD_THREAD_MIN_PRIORITY;
    if (type_ == RD_THREAD_TYPE_PRIMARY) {
      priority = config_->primary_thread_priority_;
      policy = config_->primary_thread_policy_;
    } else if (type_ == RD_THREAD_TYPE_SECONDARY) {
      priority = config_->secondary_thread_priority_;
      policy = config_->secondary_thread_policy_;
    }

    auto handle = thread_.native_handle();
    sched_param param = {calcPriority(priority)};
    int ret = pthread_setschedparam(handle, calcPolicy(policy), &param);
    if (0 != ret) {
      RD_ECHO("warning : set thread sched param failed, system error code is [%d]", ret);
    }
#endif
  }

  /**
     * 设置线程亲和性，仅针对linux系统
     */
  Void setAffinity(int index) {
#if defined(__linux__) && !defined(__ANDROID__)
    if (!config_->bind_cpu_enable_ || RD_CPU_NUM == 0 || index < 0) {
      return;
    }

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(index % RD_CPU_NUM, &mask);

    auto handle = thread_.native_handle();
    int ret = pthread_setaffinity_np(handle, sizeof(cpu_set_t), &mask);
    if (0 != ret) {
      RD_ECHO("warning : set thread affinity failed, system error code is [%d]", ret);
    }
#endif
  }


private:
  /**
     * 设定计算线程调度策略信息，
     * 非OTHER/RR/FIFO对应数值，统一返回OTHER类型
     * @param policy
     * @return
     */
  static int calcPolicy(int policy) {
    return (RD_THREAD_SCHED_OTHER == policy || RD_THREAD_SCHED_RR == policy || RD_THREAD_SCHED_FIFO == policy)
               ? policy
               : RD_THREAD_SCHED_OTHER;
  }


  /**
     * 设定线程优先级信息
     * 超过[min,max]范围，统一设置为min值
     * @param priority
     * @return
     */
  static int calcPriority(int priority) {
    return (priority >= RD_THREAD_MIN_PRIORITY && priority <= RD_THREAD_MAX_PRIORITY)
               ? priority
               : RD_THREAD_MIN_PRIORITY;
  }


protected:
  bool done_;                       // 线程状态标记
  bool is_init_;                    // 标记初始化状态
  bool is_running_;                 // 是否正在执行
  int type_ = 0;                    // 用于区分线程类型（主线程、辅助线程）
  unsigned long total_task_num_ = 0;// 处理的任务的数字

  AtomicQueue<Task> *pool_task_queue_;                 // 用于存放线程池中的普通任务
  AtomicPriorityQueue<Task> *pool_priority_task_queue_;// 用于存放线程池中的包含优先级任务的队列，仅辅助线程可以执行
  ThreadPoolConfigPtr config_ = nullptr;               // 配置参数信息
  std::thread thread_;
};

RD_ASYNC_NAMESPACE_END

#endif//RENDU_ASYNC_THREAD_THREAD_BASE_H_

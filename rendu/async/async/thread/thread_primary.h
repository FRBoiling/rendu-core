/*
* Created by boil on 2024/7/9.
*/

#ifndef RENDU_ASYNC_ASYNC_THREAD_THREAD_PRIMARY_H_
#define RENDU_ASYNC_ASYNC_THREAD_THREAD_PRIMARY_H_

#include "thread_base.h"

RD_NAMESPACE_BEGIN

class ThreadPrimary : public ThreadBase {
public:
  explicit ThreadPrimary() {
    index_ = RD_SECONDARY_THREAD_COMMON_ID;
    pool_threads_ = nullptr;
    type_ = RD_THREAD_TYPE_PRIMARY;
  }

protected:
  Status Init() override {
    RD_FUNCTION_BEGIN
    RD_ASSERT_INIT(false)
    RD_ASSERT_NOT_NULL(config_)

    is_init_ = true;
    buildStealTargets();
    thread_ = std::move(std::thread(&ThreadPrimary::Run, this));
    setSchedParam();
    setAffinity(index_);
    RD_FUNCTION_END
  }


  /**
     * 注册线程池相关内容，需要在init之前使用
     * @param index
     * @param poolTaskQueue
     * @param poolThreads
     * @param config
     */
  Status setThreadPoolInfo(int index,
                            AtomicQueue<Task>* poolTaskQueue,
                            std::vector<ThreadPrimary *>* poolThreads,
                            ThreadPoolConfigPtr config) {
    RD_FUNCTION_BEGIN
    RD_ASSERT_INIT(false)    // 初始化之前，设置参数
    RD_ASSERT_NOT_NULL(poolTaskQueue, poolThreads, config)

    this->index_ = index;
    this->pool_task_queue_ = poolTaskQueue;
    this->pool_threads_ = poolThreads;
    this->config_ = config;
    RD_FUNCTION_END
  }


  /**
     * 线程执行函数
     * @return
     */
  Status Run() final {
    RD_FUNCTION_BEGIN
    RD_ASSERT_INIT(true)
    RD_ASSERT_NOT_NULL(pool_threads_)

    /**
         * 线程池中任何一个primary线程为null都不可以执行
         * 防止线程初始化失败的情况，导致的崩溃
         * 理论不会走到这个判断逻辑里面
         */
    if (std::any_of(pool_threads_->begin(), pool_threads_->end(),
                    [](ThreadPrimary* thd) {
                      return nullptr == thd;
                    })) {
      RD_RETURN_ERROR_STATUS("primary thread is null")
    }

    status = loopProcess();
    RD_FUNCTION_END
  }


  Void processTask() override {
    Task task;
    if (popTask(task) || popPoolTask(task) || stealTask(task)) {
      runTask(task);
    } else {
      fatWait();
    }
  }


  Void processTasks() override {
    TaskArr tasks;
    if (popTask(tasks) || popPoolTask(tasks) || stealTask(tasks)) {
      // 尝试从主线程中获取/盗取批量task，如果成功，则依次执行
      runTasks(tasks);
    } else {
      fatWait();
    }
  }


  /**
     * 如果总是进入无task的状态，则开始休眠
     * 休眠一定时间后，然后恢复执行状态，避免出现
     */
  Void fatWait() {
    cur_empty_epoch_++;
    std::this_thread::yield();
    if (cur_empty_epoch_ >= config_->primary_thread_busy_epoch_) {
      RD_UNIQUE_LOCK lk(mutex_);
      cv_.wait_for(lk, std::chrono::milliseconds(config_->primary_thread_empty_interval_));
      cur_empty_epoch_ = 0;
    }
  }


  /**
     * 依次push到任一队列里。如果都失败，则yield，然后重新push
     * @param task
     * @return
     */
  Void pushTask(Task&& task) {
    while (!(primary_queue_.tryPush(std::move(task))
             || secondary_queue_.tryPush(std::move(task)))) {
      std::this_thread::yield();
    }
    cv_.notify_one();
  }


  /**
     * 从本地弹出一个任务
     * @param task
     * @return
     */
  bool popTask(TaskRef task) {
    return primary_queue_.tryPop(task) || secondary_queue_.tryPop(task);
  }


  /**
     * 从本地弹出一批任务
     * @param tasks
     * @return
     */
  bool popTask(TaskArrRef tasks) {
    Bool result = primary_queue_.tryPop(tasks, config_->max_local_batch_size_);
    auto leftSize = config_->max_local_batch_size_ - tasks.size();
    if (leftSize > 0) {
      // 如果凑齐了，就不需要了。没凑齐的话，就继续
      result |= (secondary_queue_.tryPop(tasks, leftSize));
    }
    return result;
  }


  /**
     * 从其他线程窃取一个任务
     * @param task
     * @return
     */
  bool stealTask(TaskRef task) {
    if (unlikely(pool_threads_->size() < config_->default_thread_size_)) {
      /**
             * 线程池还未初始化完毕的时候，无法进行steal。
             * 确保程序安全运行。
             */
      return false;
    }

    /**
         * 窃取的时候，仅从相邻的primary线程中窃取
         * 待窃取相邻的数量，不能超过默认primary线程数
         */
    for (auto& target : steal_targets_) {
      /**
            * 从线程中周围的thread中，窃取任务。
            * 如果成功，则返回true，并且执行任务。
             * steal 的时候，先从第二个队列里偷，从而降低触碰锁的概率
            */
      if (likely((*pool_threads_)[target])
              && (((*pool_threads_)[target])->secondary_queue_.trySteal(task))
          || ((*pool_threads_)[target])->primary_queue_.trySteal(task)) {
        return true;
      }
    }

    return false;
  }


  /**
     * 从其他线程盗取一批任务
     * @param tasks
     * @return
     */
  bool stealTask(TaskArrRef tasks) {
    if (unlikely(pool_threads_->size() != config_->default_thread_size_)) {
      return false;
    }

    for (auto& target : steal_targets_) {
      if (likely((*pool_threads_)[target])) {
        bool result = ((*pool_threads_)[target])->secondary_queue_.trySteal(tasks, config_->max_steal_batch_size_);
        auto leftSize = config_->max_steal_batch_size_ - tasks.size();
        if (leftSize > 0) {
          result |= ((*pool_threads_)[target])->primary_queue_.trySteal(tasks, leftSize);
        }

        if (result) {
          /**
                     * 在这里，我们对模型进行了简化。实现的思路是：
                     * 尝试从邻居主线程(先secondary，再primary)中，获取 x(=max_steal_batch_size_) 个task，
                     * 如果从某一个邻居中，获取了 y(<=x) 个task，则也终止steal的流程
                     * 且如果如果有一次批量steal成功，就认定成功
                     */
          return true;
        }
      }
    }

    return false;
  }


  /**
     * 构造 steal 范围的 target，避免每次盗取的时候，重复计算
     * @return
     */
  Void buildStealTargets() {
    steal_targets_.clear();
    for (int i = 0; i < config_->calcStealRange(); i++) {
      auto target = (index_ + i + 1) % config_->default_thread_size_;
      steal_targets_.push_back(target);
    }
    steal_targets_.shrink_to_fit();
  }

private:
  int index_;                                                    // 线程index
  int cur_empty_epoch_ = 0;                                      // 当前空转的轮数信息
  WorkStealingQueue<Task> primary_queue_;                      // 内部队列信息
  WorkStealingQueue<Task> secondary_queue_;                    // 第二个队列，用于减少触锁概率，提升性能
  std::vector<ThreadPrimary *>* pool_threads_;                  // 用于存放线程池中的线程信息
  std::vector<int> steal_targets_;                               // 被偷的目标信息

  std::mutex mutex_;
  std::condition_variable cv_;

  friend class ThreadPool;
};

using ThreadPrimaryPtr = ThreadPrimary *;

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_THREAD_THREAD_PRIMARY_H_

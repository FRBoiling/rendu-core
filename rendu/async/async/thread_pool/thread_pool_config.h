/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_ASYNC_ASYNC_THREAD_POOL_THREAD_POOL_CONFIG_H_
#define RENDU_ASYNC_ASYNC_THREAD_POOL_THREAD_POOL_CONFIG_H_

#include "async_define.h"
#include "thread_pool_define.h"

RD_NAMESPACE_BEGIN

struct ThreadPoolConfig : public Struct {
  /** 具体值含义，参考UThreadPoolDefine.h文件 */
  int default_thread_size_ = RD_DEFAULT_THREAD_SIZE;
  int secondary_thread_size_ = RD_SECONDARY_THREAD_SIZE;
  int max_thread_size_ = RD_MAX_THREAD_SIZE;
  int max_task_steal_range_ = RD_MAX_TASK_STEAL_RANGE;
  int max_local_batch_size_ = RD_MAX_LOCAL_BATCH_SIZE;
  int max_pool_batch_size_ = RD_MAX_POOL_BATCH_SIZE;
  int max_steal_batch_size_ = RD_MAX_STEAL_BATCH_SIZE;
  int primary_thread_busy_epoch_ = RD_PRIMARY_THREAD_BUSY_EPOCH;
  int primary_thread_empty_interval_ = RD_PRIMARY_THREAD_EMPTY_INTERVAL;
  int secondary_thread_ttl_ = RD_SECONDARY_THREAD_TTL;
  Sec monitor_span_ = RD_MONITOR_SPAN;
  MSec queue_emtpy_interval_ = RD_QUEUE_EMPTY_INTERVAL;
  int primary_thread_policy_ = RD_PRIMARY_THREAD_POLICY;
  int secondary_thread_policy_ = RD_SECONDARY_THREAD_POLICY;
  int primary_thread_priority_ = RD_PRIMARY_THREAD_PRIORITY;
  int secondary_thread_priority_ = RD_SECONDARY_THREAD_PRIORITY;
  bool bind_cpu_enable_ = RD_BIND_CPU_ENABLE;
  bool batch_task_enable_ = RD_BATCH_TASK_ENABLE;
  bool monitor_enable_ = RD_MONITOR_ENABLE;
  
  Status check() const {
    RD_FUNCTION_BEGIN
    if (default_thread_size_ < 0 || secondary_thread_size_ < 0) {
      RD_RETURN_ERROR_STATUS("thread size cannot less than 0")
    }

    if (default_thread_size_ + secondary_thread_size_ > max_thread_size_) {
      RD_RETURN_ERROR_STATUS("max thread size is less than default + secondary thread")
    }

    if (monitor_enable_ && monitor_span_ <= 0) {
      RD_RETURN_ERROR_STATUS("monitor span cannot less than 0")
    }
    RD_FUNCTION_END
  }

protected:
  /**
     * 计算可盗取的范围，盗取范围不能超过默认线程数-1
     * @return
     */
  int calcStealRange() const {
    int range = std::min(this->max_task_steal_range_, this->default_thread_size_ - 1);
    return range;
  }

  friend class ThreadPrimary;
  friend class ThreadSecondary;
};

using ThreadPoolConfigPtr = ThreadPoolConfig *;

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_THREAD_POOL_THREAD_POOL_CONFIG_H_

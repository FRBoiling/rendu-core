/*
* Created by boil on 2024/7/7.
*/

#ifndef RENDU_ASYNC_ASYNC_THREAD_POOL_THREAD_POOL_DEFINE_H_
#define RENDU_ASYNC_ASYNC_THREAD_POOL_THREAD_POOL_DEFINE_H_

#include "async_define.h"

#include <thread>
#if __cplusplus >= 201703L
#include <shared_mutex>
#else
#include <mutex>
#endif
#include <memory>

RD_NAMESPACE_BEGIN

static const Int RD_CPU_NUM = (Int) std::thread::hardware_concurrency();
static const Int RD_THREAD_TYPE_PRIMARY = 1;
static const Int RD_THREAD_TYPE_SECONDARY = 2;
#ifndef _WIN32
static const Int RD_THREAD_SCHED_OTHER = SCHED_OTHER;
static const Int RD_THREAD_SCHED_RR = SCHED_RR;
static const Int RD_THREAD_SCHED_FIFO = SCHED_FIFO;
#else
/** 线程调度策略，暂不支持windows系统 */
static const Int RD_THREAD_SCHED_OTHER = 0;
static const Int RD_THREAD_SCHED_RR = 0;
static const Int RD_THREAD_SCHED_FIFO = 0;
#endif
static const Int RD_THREAD_MIN_PRIORITY = 0;            // 线程最低优先级
static const Int RD_THREAD_MAX_PRIORITY = 99;           // 线程最高优先级
static const MSec RD_MAX_BLOCK_TTL = 1999999999;        // 最大阻塞时间，单位为ms
static const UInt RD_DEFAULT_RINGBUFFER_SIZE = 1024;    // 默认环形队列的大小
static const Index RD_SECONDARY_THREAD_COMMON_ID = -1;  // 辅助线程统一id标识
static const Int RD_DEFAULT_PRIORITY = 0;               // 默认优先级


static const Int RD_DEFAULT_TASK_STRATEGY = -1;         // 默认线程调度策略
static const Int RD_POOL_TASK_STRATEGY = -2;            // 固定用pool中的队列的调度策略
static const Int RD_LONG_TIME_TASK_STRATEGY = -101;     // 长时间任务调度策略

/**
 * 以下为线程池配置信息
 */
static const Int RD_DEFAULT_THREAD_SIZE = 8;                           // 默认开启主线程个数
static const Int RD_SECONDARY_THREAD_SIZE = 0;                         // 默认开启辅助线程个数
static const Int RD_MAX_THREAD_SIZE = 16;                              // 最大线程个数
static const Int RD_MAX_TASK_STEAL_RANGE = 2;                          // 盗取机制相邻范围
static const Bool RD_BATCH_TASK_ENABLE = false;                        // 是否开启批量任务功能
static const Int RD_MAX_LOCAL_BATCH_SIZE = 2;                          // 批量执行本地任务最大值
static const Int RD_MAX_POOL_BATCH_SIZE = 2;                           // 批量执行通用任务最大值
static const Int RD_MAX_STEAL_BATCH_SIZE = 2;                          // 批量盗取任务最大值
static const Int RD_PRIMARY_THREAD_BUSY_EPOCH = 10;                    // 主线程进入wait状态的轮数，数值越大，理论性能越高，但空转可能性也越大
static const MSec RD_PRIMARY_THREAD_EMPTY_INTERVAL = 3;                // 主线程进入休眠状态的默认时间
static const Int RD_SECONDARY_THREAD_TTL = 10;                         // 辅助线程ttl，单位为s
static const Bool RD_MONITOR_ENABLE = false;                           // 是否开启监控程序
static const Sec RD_MONITOR_SPAN = 5;                                  // 监控线程执行间隔，单位为s
static const MSec RD_QUEUE_EMPTY_INTERVAL = 3;                         // 队列为空时，等待的时间。仅针对辅助线程，单位为ms
static const Bool RD_BIND_CPU_ENABLE = false;                          // 是否开启绑定cpu模式（仅针对主线程）
static const Int RD_PRIMARY_THREAD_POLICY = RD_THREAD_SCHED_OTHER;     // 主线程调度策略
static const Int RD_SECONDARY_THREAD_POLICY = RD_THREAD_SCHED_OTHER;   // 辅助线程调度策略
static const Int RD_PRIMARY_THREAD_PRIORITY = RD_THREAD_MIN_PRIORITY;  // 主线程调度优先级（取值范围0~99，配合调度策略一起使用，不建议不了解相关内容的童鞋做修改）
static const Int RD_SECONDARY_THREAD_PRIORITY = RD_THREAD_MIN_PRIORITY;// 辅助线程调度优先级（同上）

RD_NAMESPACE_END

#endif//RENDU_ASYNC_ASYNC_THREAD_POOL_THREAD_POOL_DEFINE_H_
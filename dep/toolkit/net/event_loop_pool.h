/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_EVENT_LOOP_POOL_H
#define RENDU_EVENT_LOOP_POOL_H

#include "task/task_executor_getter.h"
#include "event_loop.h"
#include "utils/instance_imp.h"

RD_NAMESPACE_BEGIN

  class EventLoopPool : public std::enable_shared_from_this<EventLoopPool>, public TaskExecutorGetter {
  public:
    using Ptr = std::shared_ptr<EventLoopPool>;
    static const std::string kOnStarted;
#define EventPollerPoolOnStartedArgs EventLoopPool &pool, size_t &size

    ~EventLoopPool() = default;

    /**
     * 获取单例
     * @return
     */
    static EventLoopPool &Instance();

    /**
     * 设置EventPoller个数，在EventPollerPool单例创建前有效
     * 在不调用此方法的情况下，默认创建thread::hardware_concurrency()个EventPoller实例
     * @param size  EventPoller个数，如果为0则为thread::hardware_concurrency()
     */
    static void setPoolSize(size_t size = 0);

    /**
     * 内部创建线程是否设置cpu亲和性，默认设置cpu亲和性
     */
    static void enableCpuAffinity(bool enable);

    /**
     * 获取第一个实例
     * @return
     */
    EventLoop::Ptr getFirstPoller();

    /**
     * 根据负载情况获取轻负载的实例
     * 如果优先返回当前线程，那么会返回当前线程
     * 返回当前线程的目的是为了提高线程安全性
     * @param prefer_current_thread 是否优先获取当前线程
     */
    EventLoop::Ptr getPoller(bool prefer_current_thread = true);

    /**
     * 设置 GetPoller() 是否优先返回当前线程
     * 在批量创建Socket对象时，如果优先返回当前线程，
     * 那么将导致负载不够均衡，所以可以暂时关闭然后再开启
     * @param flag 是否优先返回当前线程
     */
    void preferCurrentThread(bool flag = true);

  private:
    EventLoopPool();

  private:
    bool _prefer_current_thread = true;
  };


RD_NAMESPACE_END

#endif //RENDU_EVENT_LOOP_POOL_H

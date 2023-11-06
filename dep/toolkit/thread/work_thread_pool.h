/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_WORK_THREAD_POOL_H
#define RENDU_WORK_THREAD_POOL_H

#include <memory>
#include "utils/instance_imp.h"
#include "task/task_executor_getter.h"
#include "net/event_loop_pool.h"

RD_NAMESPACE_BEGIN

  class WorkThreadPool : public std::enable_shared_from_this<WorkThreadPool>, public TaskExecutorGetter {
  public:
    using Ptr = std::shared_ptr<WorkThreadPool>;

    ~WorkThreadPool() override = default;

    /**
     * 获取单例
     */
    static WorkThreadPool &Instance();

    /**
     * 设置EventPoller个数，在WorkThreadPool单例创建前有效
     * 在不调用此方法的情况下，默认创建thread::hardware_concurrency()个EventPoller实例
     * @param size EventPoller个数，如果为0则为thread::hardware_concurrency()
     */
    static void SetPoolSize(size_t size = 0);

    /**
     * 内部创建线程是否设置cpu亲和性，默认设置cpu亲和性
     */
    static void EnableCpuAffinity(bool enable);

    /**
     * 获取第一个实例
     * @return
     */
    EventLoop::Ptr GetFirstPoller();

    /**
     * 根据负载情况获取轻负载的实例
     * 如果优先返回当前线程，那么会返回当前线程
     * 返回当前线程的目的是为了提高线程安全性
     * @return
     */
    EventLoop::Ptr GetPoller();

  protected:
    WorkThreadPool();
  };


RD_NAMESPACE_END

#endif //RENDU_WORK_THREAD_POOL_H

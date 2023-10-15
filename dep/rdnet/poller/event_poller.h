/*
* Created by boil on 2023/10/24.
*/

#ifndef RENDU_EVENT_POLLER_H
#define RENDU_EVENT_POLLER_H

#include "define.h"
#include "task/task_cancelable.h"
#include "task/task_executor.h"
#include "task/task_executor_getter.h"
#include "utils/any_storage.h"
#include "thread/semaphore.h"
#include "thread/pipe_wrapper.h"
#include "thread/thread_helper.h"
#include "logger/logger.h"
#include "buffer/buffer_raw.h"

RD_NAMESPACE_BEGIN
  class EventLoop;

  class IEventPoller {
  public:
    IEventPoller(EventLoop* eventLoop) : _event_loop(eventLoop) {}

    virtual ~IEventPoller() {};

  public:
    virtual int Create(int set_size) = 0;

    virtual int Resize(int set_size) = 0;

    virtual void Free() = 0;

    virtual int AddEvent(int fd, int mask) = 0;

    virtual void DelEvent(int fd, int mask) = 0;

    virtual int Poll(timeval *timeout) = 0;

  protected:
    EventLoop *_event_loop;
  };


  class EventPoller : public TaskExecutor, public AnyStorage, public std::enable_shared_from_this<EventPoller> {
  public:
    friend class TaskExecutorGetter;

    using Ptr = std::shared_ptr<EventPoller>;

    using DelayTask = TaskCancelable<uint64_t(void)>;

    using PollEventCallBack = std::function<void(int event)>;
    using PollDelCallBack = std::function<void(bool success)>;

    typedef enum {
      Event_Read = 1 << 0, //读事件
      Event_Write = 1 << 1, //写事件
      Event_Error = 1 << 2, //错误事件
      Event_LT = 1 << 3,//水平触发
    } Poll_Event;

    ~EventPoller();

    /**
     * 获取EventPollerPool单例中的第一个EventPoller实例，
     * 保留该接口是为了兼容老代码
     * @return 单例
     */
    static EventPoller &Instance();

    /**
     * 添加事件监听
     * @param fd 监听的文件描述符
     * @param event 事件类型，例如 Event_Read | Event_Write
     * @param cb 事件回调functional
     * @return -1:失败，0:成功
     */
    int AddEvent(int fd, int event, PollEventCallBack cb);

    /**
     * 删除事件监听
     * @param fd 监听的文件描述符
     * @param callback 删除成功回调functional
     * @return -1:失败，0:成功
     */
    int RemoveEvent(int fd, PollDelCallBack callback = nullptr);

    /**
     * 修改监听事件类型
     * @param fd 监听的文件描述符
     * @param event 事件类型，例如 Event_Read | Event_Write
     * @return -1:失败，0:成功
     */
    int modifyEvent(int fd, int event);

    /**
     * 异步执行任务
     * @param task 任务
     * @param may_sync 如果调用该函数的线程就是本对象的轮询线程，那么may_sync为true时就是同步执行任务
     * @return 是否成功，一定会返回true
     */
    Task::Ptr Async(TaskIn task, bool may_sync = true) override;

    /**
     * 同async方法，不过是把任务打入任务列队头，这样任务优先级最高
     * @param task 任务
     * @param may_sync 如果调用该函数的线程就是本对象的轮询线程，那么may_sync为true时就是同步执行任务
     * @return 是否成功，一定会返回true
     */
    Task::Ptr AsyncFirst(TaskIn task, bool may_sync = true) override;

    /**
     * 判断执行该接口的线程是否为本对象的轮询线程
     * @return 是否为本对象的轮询线程
     */
    bool isCurrentThread();

    /**
     * 延时执行某个任务
     * @param delay_ms 延时毫秒数
     * @param task 任务，返回值为0时代表不再重复任务，否则为下次执行延时，如果任务中抛异常，那么默认不重复任务
     * @return 可取消的任务标签
     */
    DelayTask::Ptr doDelayTask(uint64_t delay_ms, std::function<uint64_t()> task);

    /**
     * 获取当前线程关联的Poller实例
     */
    static EventPoller::Ptr getCurrentPoller();

    /**
     * 获取当前线程下所有socket共享的读缓存
     */
    BufferRaw::Ptr getSharedBuffer();

    /**
     * 获取poller线程id
     */
    std::thread::id getThreadId() const;

    /**
     * 获取线程名
     */
    const std::string& getThreadName() const;

  private:
    /**
     * 本对象只允许在EventPollerPool中构造
     */
    EventPoller(std::string name);

    /**
     * 执行事件轮询
     * @param blocked 是否用执行该接口的线程执行轮询
     * @param ref_self 是记录本对象到thread local变量
     */
    void runLoop(bool blocked, bool ref_self);

    /**
     * 内部管道事件，用于唤醒轮询线程用
     */
    void onPipeEvent();

    /**
     * 切换线程并执行任务
     * @param task
     * @param may_sync
     * @param first
     * @return 可取消的任务本体，如果已经同步执行，则返回nullptr
     */
    Task::Ptr async_l(TaskIn task, bool may_sync = true, bool first = false);

    /**
     * 结束事件轮询
     * 需要指出的是，一旦结束就不能再次恢复轮询线程
     */
    void shutdown();

    /**
     * 刷新延时任务
     */
    uint64_t flushDelayTask(uint64_t now);

    /**
     * 获取select或epoll休眠时间
     */
    uint64_t getMinDelay();

    /**
     * 添加管道监听事件
     */
    void addEventPipe();

  private:
    class ExitException : public std::exception {};

  private:
    //标记loop线程是否退出
    bool _exit_flag;
    //线程名
    std::string _name;
    //当前线程下，所有socket共享的读缓存
    std::weak_ptr<BufferRaw> _shared_buffer;
    //执行事件循环的线程
    std::thread *_loop_thread = nullptr;
    //通知事件循环的线程已启动
    Semaphore _sem_run_started;

    //内部事件管道
    PipeWrapper _pipe;
    //从其他线程切换过来的任务
    std::mutex _mtx_task;
    List<Task::Ptr> _list_task;

    //保持日志可用
    Logger::Ptr _logger;

#if defined(HAS_EPOLL)
    //epoll相关
    int _epoll_fd = -1;
    unordered_map<int, std::shared_ptr<PollEventCB> > _event_map;
#else
    //select相关
    struct PollRecord {
      using Ptr = std::shared_ptr<PollRecord>;
      int m_event;
      int m_attach;
      PollEventCallBack m_call_back;
    };
    unordered_map<int, PollRecord::Ptr> _event_map;
#endif //HAS_EPOLL

    //定时器相关
    std::multimap<uint64_t, DelayTask::Ptr> _delay_task_map;
  };

RD_NAMESPACE_END

#endif //RENDU_EVENT_POLLER_H

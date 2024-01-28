/*
* Created by boil on 2023/10/26.
*/

#ifndef RENDU_SOCKET_WRAPPER_H
#define RENDU_SOCKET_WRAPPER_H

#include "socket_sender.h"
#include "task/a_task_executor.h"
#include "socket.h"

RD_NAMESPACE_BEGIN

//Socket对象的包装类
  class SocketWrapper
    : public SocketSender, public ISocket, public ATaskExecutor, public std::enable_shared_from_this<SocketWrapper> {
  public:

    SocketWrapper(const Socket::Ptr &sock);

    virtual ~SocketWrapper() override = default;

    using Ptr = std::shared_ptr<SocketWrapper>;

    ///////////////////// Socket util std::functions /////////////////////
    /**
     * 获取poller线程
     */
    const EventLoop::Ptr &getPoller() const;

    /**
     * 设置批量发送标记,用于提升性能
     * @param try_flush 批量发送标记
     */
    void setSendFlushFlag(bool try_flush);

    /**
     * 设置socket发送flags
     * @param flags socket发送flags
     */
    void setSendFlags(int flags);

    /**
     * 套接字是否忙，如果套接字写缓存已满则返回true
     */
    bool isSocketBusy() const;

    /**
     * 设置Socket创建器，自定义Socket创建方式
     * @param cb 创建器
     */
    void setOnCreateSocket(Socket::onCreateSocket cb);

    /**
     * 创建socket对象
     */
    Socket::Ptr createSocket();

    /**
     * 获取socket对象
     */
    const Socket::Ptr &getSock() const;

    /**
     * 尝试将所有数据写socket
     * @return -1代表失败(socket无效或者发送超时)，0代表成功?
     */
    int flushAll();

    /**
     * 是否ssl加密
     */
    virtual bool overSsl() const { return false; }

    ///////////////////// SockInfo override /////////////////////
    std::string GetLocalIp() override;

    uint16_t GetLocalPort() override;

    std::string GetRemoteIp() override;

    uint16_t GetRemotePort() override;

    ///////////////////// TaskExecutorInterface override /////////////////////
    /**
     * 任务切换到所属poller线程执行
     * @param task 任务
     * @param may_sync 是否运行同步执行任务
     */
    Task::Ptr Async(TaskIn task, bool may_sync = true) override;

    Task::Ptr AsyncFirst(TaskIn task, bool may_sync = true) override;

    ///////////////////// SockSender override /////////////////////
    /**
     * 统一发送数据的出口
     */
    ssize_t Send(ABuffer::Ptr buf) override;

    /**
     * 触发onErr事件
     */
    void Shutdown(const SockException &ex = SockException(Err_shutdown, "self shutdown")) override;

    /**
     * 线程安全的脱离 Server 并触发 onError 事件
     * @param ex 触发 onError 事件的原因
     */
    void safeShutdown(const SockException &ex = SockException(Err_shutdown, "self shutdown"));

    ///////////////////// event functions /////////////////////
    /**
     * 连接服务器结果回调
     * @param ex 成功与否
     */
    virtual void onConnect(const SockException &ex) = 0;
      /**
       * 接收数据入口
       * @param buf 数据，可以重复使用内存区,不可被缓存使用
       */
    virtual void onRecv(const ABuffer::Ptr &buf) = 0;

    /**
     * 收到 eof 或其他导致脱离 Server 事件的回调
     * 收到该事件时, 该对象一般将立即被销毁
     * @param err 原因
     */
    virtual void onError(const SockException &err) = 0;

    /**
     * 数据全部发送完毕后回调
     */
    virtual void onFlush() = 0;

    /**
     * 每隔一段时间触发, 用来做超时管理
     */
    virtual void onManager() = 0;

    IPEndPoint *GetEndPoint() override;

    void Bind(IPEndPoint *endPoint) override;

    void Close() override;

    void Listen(int backlog) override;

    void Connect(const char *remote_host, uint16_t remote_port) override;

    ISocket *Accept() override;

    /**
    * 唯一标识
    */
    string GetIdentifier() const override;

  protected:
    void setPoller(const EventLoop::Ptr &poller);

    void setSock(const Socket::Ptr &sock);

  private:
    bool _try_flush = true;
    Socket::Ptr _sock;
    EventLoop::Ptr _poller;
    Socket::onCreateSocket _on_create_socket;
  };

RD_NAMESPACE_END

#endif //RENDU_SOCKET_WRAPPER_H

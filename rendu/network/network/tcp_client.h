/*
* Created by boil on 2023/11/6.
*/

#ifndef RENDU_TCP_CLIENT_H
#define RENDU_TCP_CLIENT_H


#include <map>

#include "base/non_copyable.h"
#include "base/atomic.h"

#include "event_loop.h"
#include "endpoint/ip_end_point.h"
#include "event_loop_thread_pool.h"

RD_NAMESPACE_BEGIN

  class Connector;

  typedef std::shared_ptr<Connector> ConnectorPtr;

  class TcpClient : NonCopyable {
  public:
    // TcpClient(EventLoop* loop);
    // TcpClient(EventLoop* loop, const string& host, uint16_t port);
    TcpClient(EventLoop *loop,
              const IPEndPoint &serverAddr,
              const string &nameArg);

    ~TcpClient();  // force out-line dtor, for std::unique_ptr members.

    void connect();

    void disconnect();

    void stop();

    TcpConnectionPtr connection() const {
      MutexLockGuard lock(mutex_);
      return connection_;
    }

    EventLoop *getLoop() const { return loop_; }

    bool retry() const { return retry_; }

    void enableRetry() { retry_ = true; }

    const string &name() const { return name_; }

    /// Set connection callback.
    /// Not thread safe.
    void setConnectionCallback(ConnectionCallback cb) { connectionCallback_ = std::move(cb); }

    /// Set message callback.
    /// Not thread safe.
    void setMessageCallback(MessageCallback cb) { messageCallback_ = std::move(cb); }

    /// Set write complete callback.
    /// Not thread safe.
    void setWriteCompleteCallback(WriteCompleteCallback cb) { writeCompleteCallback_ = std::move(cb); }

  private:
    /// Not thread safe, but in loop
    void newConnection(int sockfd);

    /// Not thread safe, but in loop
    void removeConnection(const TcpConnectionPtr &conn);

    EventLoop *loop_;
    ConnectorPtr connector_; // avoid revealing Connector
    const string name_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    bool retry_;   // atomic
    bool connect_; // atomic
    // always in loop thread
    int nextConnId_;
    mutable MutexLock mutex_;
    TcpConnectionPtr connection_ GUARDED_BY(mutex_);
  };

RD_NAMESPACE_END

#endif //RENDU_TCP_CLIENT_H

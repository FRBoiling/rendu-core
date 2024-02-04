///*
//* Created by boil on 2023/11/3.
//*/
//
//#ifndef RENDU_NET_TCP_SERVER_H
//#define RENDU_NET_TCP_SERVER_H
//
//#include <map>
//
//#include "base/atomic/atomic.h"
//#include "base/utils/non_copyable.h"
//
//#include "event_loop.h"
//#include "endpoint/ip_end_point.h"
//#include "event_loop_thread_pool.h"
//#include "acceptor.h"
//
//NET_NAMESPACE_BEGIN
//
//class TcpServer : NonCopyable
//{
//public:
//  typedef std::function<void(EventLoop*)> ThreadInitCallback;
//  enum Option
//  {
//    kNoReusePort,
//    kReusePort,
//  };
//
//  TcpServer(EventLoop* loop,
//            const IPEndPoint& listenAddr,
//            const std::string& nameArg,
//            Option option = kNoReusePort);
//  ~TcpServer();  // force out-line dtor, for std::unique_ptr members.
//
//  const std::string& IpPort() const { return ipPort_; }
//  const std::string& Name() const { return name_; }
//  EventLoop* GetLoop() const { return loop_; }
//
//  /// Set the number of threads for handling input.
//  ///
//  /// Always accepts new connection in loop's thread.
//  /// Must be called before @c start
//  /// @param numThreads
//  /// - 0 means all I/O in loop's thread, no thread will created.
//  ///   this is the default value.
//  /// - 1 means all I/O in another thread.
//  /// - N means a thread pool with N threads, new connections
//  ///   are assigned on a round-robin basis.
//  void SetThreadNum(int numThreads);
//  void SetThreadInitCallback(const ThreadInitCallback& cb)
//  { threadInitCallback_ = cb; }
//  /// valid after calling start()
//  std::shared_ptr<EventLoopThreadPool> threadPool()
//  { return threadPool_; }
//
//  /// Starts the server if it's not listening.
//  ///
//  /// It's harmless to call it multiple times.
//  /// Thread safe.
//  void Start();
//
//  /// Set connection callback.
//  /// Not thread safe.
//  void SetConnectionCallback(const ConnectionCallback& cb)
//  { connectionCallback_ = cb; }
//
//  /// Set message callback.
//  /// Not thread safe.
//  void SetMessageCallback(const MessageCallback& cb)
//  { messageCallback_ = cb; }
//
//  /// Set write complete callback.
//  /// Not thread safe.
//  void SetWriteCompleteCallback(const WriteCompleteCallback& cb)
//  { writeCompleteCallback_ = cb; }
//
//private:
//  /// Not thread safe, but in loop
//  void newConnection(int sockfd, const IPEndPoint& peerAddr);
//  /// Thread safe.
//  void removeConnection(const TcpConnectionPtr& conn);
//  /// Not thread safe, but in loop
//  void removeConnectionInLoop(const TcpConnectionPtr& conn);
//private:
//  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
//
//  EventLoop* loop_;  // the acceptor loop
//  const std::string ipPort_;
//  const std::string name_;
//  std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
//  std::shared_ptr<EventLoopThreadPool> threadPool_;
//  ConnectionCallback connectionCallback_;
//  MessageCallback messageCallback_;
//  WriteCompleteCallback writeCompleteCallback_;
//  ThreadInitCallback threadInitCallback_;
//  AtomicInt32 started_;
//  // always in loop thread
//  int nextConnId_;
//  ConnectionMap connections_;
//};
//
//NET_NAMESPACE_END
//
//#endif //RENDU_NET_TCP_SERVER_H

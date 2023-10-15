/*
* Created by boil on 2023/11/3.
*/

#ifndef RENDU_CONNECTOR_H
#define RENDU_CONNECTOR_H

#include "base/non_copyable.h"
#include "endpoint/ip_end_point.h"

RD_NAMESPACE_BEGIN

  class Channel;

  class EventLoop;

  class Connector : NonCopyable,
                    public std::enable_shared_from_this<Connector> {
  public:
    typedef std::function<void(int sockfd)> NewConnectionCallback;

    Connector(EventLoop *loop, const IPEndPoint &serverAddr);

    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback &cb) { newConnectionCallback_ = cb; }

    void start();  // can be called in any thread
    void restart();  // must be called in loop thread
    void stop();  // can be called in any thread

    const IPEndPoint &serverAddress() const { return serverAddr_; }

  private:
    enum States {
      kDisconnected, kConnecting, kConnected
    };
    static const int kMaxRetryDelayMs = 30 * 1000;
    static const int kInitRetryDelayMs = 500;

    void setState(States s) { state_ = s; }

    void startInLoop();

    void stopInLoop();

    void connect();

    void connecting(int sockfd);

    void handleWrite();

    void handleError();

    void retry(int sockfd);

    int removeAndResetChannel();

    void resetChannel();

    EventLoop *loop_;
    IPEndPoint serverAddr_;
    bool connect_; // atomic
    States state_;  // FIXME: use atomic variable
    std::unique_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    int retryDelayMs_;
  };

RD_NAMESPACE_END

#endif //RENDU_CONNECTOR_H

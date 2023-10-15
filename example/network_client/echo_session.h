/*
* Created by boil on 2023/10/27.
*/

#ifndef RENDU_ECHO_SERVER_H
#define RENDU_ECHO_SESSION_H

#include "session/session.h"

RD_NAMESPACE_BEGIN

  class EchoSession : public Session {
  public:
    explicit EchoSession(const Socket::Ptr &sock);

    ~EchoSession() override;

  public:
    void onRecv(const ABuffer::Ptr &buf) override;

    void onError(const SockException &ex) override;

    void onConnect(const SockException &ex) override;

    void onFlush() override;

    void onManager() override;

  private:
    Ticker _ticker;
    int _nTick = 0;
  };

RD_NAMESPACE_END

#endif //RENDU_ECHO_SERVER_H

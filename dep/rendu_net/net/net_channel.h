/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_NET_NetChannel_H
#define RENDU_NET_NetChannel_H

#include "common/utils/non_copyable.h"
#include "socket/socket.h"
#include "net_poll_event.h"

RD_NAMESPACE_BEGIN

///
/// A selectable I/O NetChannel.
///


  class NetChannel : NonCopyable {
  public:
    using Ptr = std::shared_ptr<NetChannel>;

  public:
    typedef std::function<void(Timestamp)> EventCallback;

    NetChannel();

    ~NetChannel();

  public:
    int32 GetKey() { return m_socket->GetFD(); };

  public:
    void HandleEvent(Timestamp receiveTime);

    Socket::Ptr GetSocket() const { return m_socket; };

  private:
    void update();

  public:
    Socket::Ptr m_socket;
    NetPollEventMap m_pollEventMap;
  };

  typedef std::vector<NetChannel *> NetChannelList;
  typedef std::map<SOCKET, NetChannel *> NetChannelMap;

RD_NAMESPACE_END

#endif //RENDU_NET_NetChannel_H

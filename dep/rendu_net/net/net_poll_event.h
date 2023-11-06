/*
* Created by boil on 2023/11/11.
*/

#ifndef RENDU_NET_POLL_EVENT_H
#define RENDU_NET_POLL_EVENT_H

#include "define.h"
#include "time/timestamp.h"
#include <functional>
#include <map>

RD_NAMESPACE_BEGIN

  class NetPollEvent {
  public:
    typedef enum {
      NPE_NONE = 0, //读事件

      NPE_READ = 1 << 0, //读事件
      NPE_WRITE = 1 << 1, //写事件
      NPE_ERROR = 1 << 2, //错误事件
      NPE_LT = 1 << 3,//水平触发
    } Type;

  public:
    using CallBack = std::function<void(int Timestamp)>;
  public:
    Type m_Type;
    CallBack m_OnCallBack;
  };

  typedef std::map<NetPollEvent::Type, NetPollEvent *> NetPollEventMap;

RD_NAMESPACE_END

#endif //RENDU_NET_POLL_EVENT_H

/*
* Created by boil on 2024/3/14.
*/

#ifndef RENDU_NET_NET_POLLER_POLL_EVENT_H_
#define RENDU_NET_NET_POLLER_POLL_EVENT_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

struct PollEvent {

  PollEvent() = default;
  PollEvent(int fd, short events) : FileDescriptor(fd), Events(events){};

  int FileDescriptor;  // 文件描述符
  ushort Events;       // 关心的事件,一般用bit位来标识不同的事件，例如可读、可写、出错等
  uint TriggeredEvents;// 返回的已触发事件
};

NET_NAMESPACE_END
#endif//RENDU_NET_NET_POLLER_POLL_EVENT_H_

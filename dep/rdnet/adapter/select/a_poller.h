/*
* Created by boil on 2023/11/1.
*/

#ifndef RENDU_I_POLLER_H
#define RENDU_I_POLLER_H

#include "define.h"

RD_NAMESPACE_BEGIN

  class APoller {
  public:
    virtual int Create(int set_size) = 0;

    virtual int Resize(int set_size) = 0;

    virtual void Free() = 0;

    virtual int AddEvent(int fd, int mask) = 0;

    virtual void DelEvent(int fd, int mask) = 0;

    virtual int Poll(timeval *timeout) = 0;
  };

RD_NAMESPACE_END

#endif //RENDU_I_POLLER_H

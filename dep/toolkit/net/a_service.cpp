/*
* Created by boil on 2023/10/26.
*/

#include "a_service.h"
#include "event_loop_pool.h"

RD_NAMESPACE_BEGIN

AService::AService(EventLoop::Ptr poller) {
  _poller = poller ? std::move(poller) : EventLoopPool::Instance().getPoller();
}

RD_NAMESPACE_END
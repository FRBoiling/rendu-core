/*
* Created by boil on 2023/10/26.
*/

#include "a_service.h"
#include "poller/event_poller_pool.h"

RD_NAMESPACE_BEGIN

AService::AService(EventPoller::Ptr poller) {
  _poller = poller ? std::move(poller) : EventPollerPool::Instance().getPoller();
}

RD_NAMESPACE_END
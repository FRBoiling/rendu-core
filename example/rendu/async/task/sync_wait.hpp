///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////
#ifndef RENDU_ASYNC_SYNC_WAIT_HPP
#define RENDU_ASYNC_SYNC_WAIT_HPP

#include "detail/awaitable_traits.hpp"
#include "detail/lightweight_manual_reset_event.hpp"
#include "detail/sync_wait_task.hpp"

#include <atomic>
#include <cstdint>

ASYNC_NAMESPACE_BEGIN

template<typename AWAITABLE>
auto sync_wait(AWAITABLE &&awaitable)
    -> typename detail::AwaitableTraits<AWAITABLE &&>::await_result_t {

  auto task = detail::make_sync_wait_task(std::forward<AWAITABLE>(awaitable));
  detail::lightweight_manual_reset_event event;
  task.start(event);
  event.wait();
  return task.result();
}

ASYNC_NAMESPACE_END

#endif

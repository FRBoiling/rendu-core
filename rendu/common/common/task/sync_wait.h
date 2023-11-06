/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_COMMON_SYNC_WAIT_H
#define RENDU_COMMON_SYNC_WAIT_H

#include "common/common_define.h"
#include "common/task/detail/awaitable_traits.h"
#include "common/task/detail/sync_wait_task.h"

COMMON_NAMESPACE_BEGIN

    template<typename AWAITABLE>
    auto sync_wait(AWAITABLE &&awaitable)
    -> typename awaitable_traits<AWAITABLE &&>::await_result_t {
      auto task = detail::make_sync_wait_task(std::forward<AWAITABLE>(awaitable));
      detail::lightweight_manual_reset_event event;
      task.start(event);
      event.wait();
      return task.result();
    }

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_SYNC_WAIT_H

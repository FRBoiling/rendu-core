/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_COMMON_WHEN_ALL_READY_H
#define RENDU_COMMON_WHEN_ALL_READY_H

#include "common/common_define.h"
#include "common/task/detail/awaitable_traits.h"
#include "common/task/detail/unwrap_reference.h"
#include "common/task/detail/when_all_ready_awaitable.h"
#include "common/task/detail/when_all_task.h"
#include "is_awaitable.h"
#include "task.h"

COMMON_NAMESPACE_BEGIN
    template<
        typename... AWAITABLES,
        std::enable_if_t<std::conjunction_v<
            is_awaitable<detail::unwrap_reference_t<std::remove_reference_t<AWAITABLES>>>...>, int> = 0>
    [[nodiscard]]
    inline auto when_all_ready(AWAITABLES &&... awaitables) {
      return detail::when_all_ready_awaitable<std::tuple<detail::when_all_task<
          typename awaitable_traits<detail::unwrap_reference_t<std::remove_reference_t<AWAITABLES>>>::await_result_t>...>>(
          std::make_tuple(detail::make_when_all_task(std::forward<AWAITABLES>(awaitables))...));
    }

// TODO: Generalise this from vector<AWAITABLE> to arbitrary sequence of awaitable.

    template<
        typename AWAITABLE,
        typename RESULT = typename awaitable_traits<detail::unwrap_reference_t<AWAITABLE>>::await_result_t>
    [[nodiscard]] auto when_all_ready(std::vector<AWAITABLE> awaitables) {
      std::vector<detail::when_all_task<RESULT>> tasks;

      tasks.reserve(awaitables.size());

      for (auto &awaitable: awaitables) {
        tasks.emplace_back(detail::make_when_all_task(std::move(awaitable)));
      }

      return detail::when_all_ready_awaitable<std::vector<detail::when_all_task<RESULT>>>(
          std::move(tasks));
    }

COMMON_NAMESPACE_END
#endif //RENDU_COMMON_WHEN_ALL_READY_H

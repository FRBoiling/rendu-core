/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_LIGHTWEIGHT_MANUAL_RESET_EVENT_H
#define RENDU_LIGHTWEIGHT_MANUAL_RESET_EVENT_H

#include "async_define.h"
#include <coroutine>
#include <mutex>
#include <condition_variable>

ASYNC_NAMESPACE_BEGIN
    namespace detail {
        class lightweight_manual_reset_event {
        public:

          lightweight_manual_reset_event(bool initiallySet = false);

          ~lightweight_manual_reset_event();

          void set() noexcept;

          void reset() noexcept;

          void wait() noexcept;

        private:

          // For other platforms that don't have a native futex
          // or manual reset event we can just use a std::mutex
          // and std::condition_variable to perform the wait.
          // Not so lightweight, but should be portable to all platforms.
          std::mutex m_mutex;
          std::condition_variable m_cv;
          bool m_isSet;
        };
    }
ASYNC_NAMESPACE_END

#endif //RENDU_LIGHTWEIGHT_MANUAL_RESET_EVENT_H

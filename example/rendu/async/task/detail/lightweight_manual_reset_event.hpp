/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_DETAIL_LIGHTWEIGHT_MANUAL_RESET_EVENT_HPP
#define RENDU_ASYNC_DETAIL_LIGHTWEIGHT_MANUAL_RESET_EVENT_HPP

#include "async_define.h"

#include <condition_variable>
#include <mutex>

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
#if RENDU_ASYNC_OS_LINUX
    std::atomic<int> m_value;
#elif RENDU_ASYNC_OS_WINNT >= 0x0602
    // Windows 8 or newer we can use WaitOnAddress()
    std::atomic<std::uint8_t> m_value;
#elif RENDU_ASYNC_OS_WINNT
    // Before Windows 8 we need to use a WIN32 manual reset event.
    cppcoro::detail::win32::handle_t m_eventHandle;
#else
    // For other platforms that don't have a native futex
    // or manual reset event we can just use a std::mutex
    // and std::condition_variable to perform the wait.
    // Not so lightweight, but should be portable to all platforms.
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_isSet;
#endif
  };
}// namespace detail
ASYNC_NAMESPACE_END

#endif

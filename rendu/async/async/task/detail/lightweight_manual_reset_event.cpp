/*
* Created by boil on 2023/10/13.
*/

#include "lightweight_manual_reset_event.h"
ASYNC_NAMESPACE_BEGIN

detail::lightweight_manual_reset_event::lightweight_manual_reset_event(bool initiallySet)
    : m_isSet(initiallySet)
{
}

detail::lightweight_manual_reset_event::~lightweight_manual_reset_event()
{
}

void detail::lightweight_manual_reset_event::set() noexcept
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_isSet = true;
  m_cv.notify_all();
}

void detail::lightweight_manual_reset_event::reset() noexcept
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_isSet = false;
}

void detail::lightweight_manual_reset_event::wait() noexcept
{
  std::unique_lock<std::mutex> lock(m_mutex);
  m_cv.wait(lock, [this] { return m_isSet; });
}


ASYNC_NAMESPACE_END
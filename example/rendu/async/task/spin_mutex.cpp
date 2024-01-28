/*
* Created by boil on 2024/01/01.
*/

#include "spin_mutex.hpp"
#include "spin_wait.hpp"

ASYNC_NAMESPACE_BEGIN

spin_mutex::spin_mutex() noexcept
    : m_isLocked(false) {
}

bool spin_mutex::try_lock() noexcept {
  return !m_isLocked.exchange(true, std::memory_order_acquire);
}

void spin_mutex::lock() noexcept {
  spin_wait wait;
  while (!try_lock()) {
    while (m_isLocked.load(std::memory_order_relaxed)) {
      wait.spin_one();
    }
  }
}

void spin_mutex::unlock() noexcept {
  m_isLocked.store(false, std::memory_order_release);
}

ASYNC_NAMESPACE_END

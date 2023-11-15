/*
* Created by boil on 2023/10/13.
*/

#include "spin_wait.h"
#include <thread>

namespace {
    namespace local {
        constexpr std::uint32_t yield_threshold = 10;
    }
}


ASYNC_NAMESPACE_BEGIN


    spin_wait::spin_wait() noexcept {
      reset();
    }

    bool spin_wait::next_spin_will_yield() const noexcept {
      return m_count >= local::yield_threshold;
    }

    void spin_wait::reset() noexcept {
      static const std::uint32_t initialCount =
          std::thread::hardware_concurrency() > 1 ? 0 : local::yield_threshold;
      m_count = initialCount;
    }

    void spin_wait::spin_one() noexcept {

      if (next_spin_will_yield()) {
        std::this_thread::yield();
      }

      ++m_count;
      if (m_count == 0) {
        // Don't wrap around to zero as this would go back to
        // busy-waiting.
        m_count = local::yield_threshold;
      }
    }


ASYNC_NAMESPACE_END

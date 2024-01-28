/*
* Created by boil on 2024/01/01.
*/

#ifndef RENDU_ASYNC_SPIN_WAIT_HPP
#define RENDU_ASYNC_SPIN_WAIT_HPP

#include "async_define.h"
#include <cstdint>

ASYNC_NAMESPACE_BEGIN

class spin_wait {
public:
  spin_wait() noexcept;

  bool next_spin_will_yield() const noexcept;

  void spin_one() noexcept;

  void reset() noexcept;

private:
  std::uint32_t m_count;
};

ASYNC_NAMESPACE_END

#endif

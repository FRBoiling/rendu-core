/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_COMMON_SPIN_WAIT_H
#define RENDU_COMMON_SPIN_WAIT_H

#include "common_define.h"
#include <cstdint>

COMMON_NAMESPACE_BEGIN

    class spin_wait
    {
    public:

      spin_wait() noexcept;

      bool next_spin_will_yield() const noexcept;

      void spin_one() noexcept;

      void reset() noexcept;

    private:

      std::uint32_t m_count;

    };

COMMON_NAMESPACE_END

#endif //RENDU_COMMON_SPIN_WAIT_H

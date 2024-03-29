/*
* Created by boil on 2023/10/13.
*/

#ifndef RENDU_SPIN_WAIT_H
#define RENDU_SPIN_WAIT_H

#include "define.h"
#include <cstdint>

RD_NAMESPACE_BEGIN

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

RD_NAMESPACE_END

#endif //RENDU_SPIN_WAIT_H

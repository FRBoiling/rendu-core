#ifndef RENDU_UTIL_SLEEP_H_
#define RENDU_UTIL_SLEEP_H_

#include <cstdint>
#include <chrono>
#include <thread>

namespace rendu::utils
{
    inline void Sleep(const std::uint64_t milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    template <typename Rep, typename Period>
    void Sleep(const std::chrono::duration<Rep, Period>& time) {
        std::this_thread::sleep_for(time);
    }
}

#endif // RENDU_CORE_UTIL_SLEEP_H_
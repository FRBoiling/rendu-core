#ifndef RENDU_COMMON_PLATFORM_ASSERT_H_
#define RENDU_COMMON_PLATFORM_ASSERT_H_

#include "basic_loggers_def.h"

namespace rendu::log
{
    template <DefinitionLogger Logger, typename... Args>
    void Console(const Formater<Logger> format, Args&&... args) noexcept;

    template <typename... Args>
    void Console(const traits::DefinitionLogger auto logger, const Formater<std::decay_t<decltype(logger)>> format, Args&&... args) noexcept;
}

#ifdef RENDU_DEBUG
    #include "platform.h"
    #include <tuple>

    #define RENDU_ASSERT(cond, ...)                                                    \
        do {                                                                            \
            if(!(cond)) {                                                               \
                rendu::log::Console<rendu::log::Assert>("Condition: " #cond);         \
                                                                                        \
                using tuple = decltype(std::forward_as_tuple(__VA_ARGS__));             \
                if constexpr(std::tuple_size_v<tuple> > 0) {                            \
                    rendu::log::Console<rendu::log::Assert>("Message: " __VA_ARGS__); \
                }                                                                       \
                                                                                        \
                if(RENDU_DEBUGGING()) {                                                \
                    RENDU_BREAKPOINT();                                                \
                }                                                                       \
                                                                                        \
                std::exit(EXIT_FAILURE);                                                \
            }                                                                           \
        } while(false)
#else
    #define RENDU_ASSERT(cond, ...)
#endif

#endif // RENDU_COMMON_PLATFORM_ASSERT_H_

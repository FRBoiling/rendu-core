#ifndef HELENA_ENGINE_LOG_H_
#define HELENA_ENGINE_LOG_H_

#include "types/basic_loggers_def.h"

namespace rendu::log
{
    template <traits::DefinitionLogger Logger, typename... Args>
    void Console(const Formater<Logger> format, Args&&... args) noexcept;

    template <typename... Args>
    void Console(const traits::DefinitionLogger auto logger, const Formater<std::decay_t<decltype(logger)>> format, Args&&... args) noexcept;
}

// NOTE MSVC: If you get an error, make sure you add a preprocessor /Zc:preprocessor for support VA_OPT
// Read it here: https://docs.microsoft.com/en-us/cpp/build/reference/zc-preprocessor?view=msvc-160
#define HELENA_MSG(prefix, fmt, ...)        Helena::Log::Console<prefix>(fmt __VA_OPT__(,) __VA_ARGS__)

#if defined(HELENA_DEBUG)
    #define HELENA_MSG_DEBUG(fmt, ...)      HELENA_MSG(Helena::Log::Debug,      fmt __VA_OPT__(,) __VA_ARGS__)
    #define HELENA_MSG_INFO(fmt, ...)       HELENA_MSG(Helena::Log::Info,       fmt __VA_OPT__(,) __VA_ARGS__)
    #define HELENA_MSG_NOTICE(fmt, ...)     HELENA_MSG(Helena::Log::Notice,     fmt __VA_OPT__(,) __VA_ARGS__)
    #define HELENA_MSG_WARNING(fmt, ...)    HELENA_MSG(Helena::Log::Warning,    fmt __VA_OPT__(,) __VA_ARGS__)
    #define HELENA_MSG_ERROR(fmt, ...)      HELENA_MSG(Helena::Log::Error,      fmt __VA_OPT__(,) __VA_ARGS__)
    #define HELENA_MSG_FATAL(fmt, ...)      HELENA_MSG(Helena::Log::Fatal,      fmt __VA_OPT__(,) __VA_ARGS__)
#else
    #define HELENA_MSG_DEBUG(fmt, ...)
    #define HELENA_MSG_INFO(fmt, ...)
    #define HELENA_MSG_NOTICE(fmt, ...)     HELENA_MSG(Helena::Log::Notice,     fmt __VA_OPT__(,) __VA_ARGS__)
    #define HELENA_MSG_WARNING(fmt, ...)    HELENA_MSG(Helena::Log::Warning,    fmt __VA_OPT__(,) __VA_ARGS__)
    #define HELENA_MSG_ERROR(fmt, ...)      HELENA_MSG(Helena::Log::Error,      fmt __VA_OPT__(,) __VA_ARGS__)
    #define HELENA_MSG_FATAL(fmt, ...)      HELENA_MSG(Helena::Log::Fatal,      fmt __VA_OPT__(,) __VA_ARGS__)
#endif

#endif // HELENA_ENGINE_LOG_H_

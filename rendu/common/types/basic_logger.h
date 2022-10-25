#ifndef RENDU_TYPES_BASIC_LOGGER_H_
#define RENDU_TYPES_BASIC_LOGGER_H_

#include "basic_loggers_def.h"

namespace rendu::types
{
    class BasicLogger
    {
        [[nodiscard]] static bool MakeColor(fmt::memory_buffer& buffer, const fmt::text_style style);
        static void EndColor(fmt::memory_buffer& buffer) noexcept;

    public:
        BasicLogger() = delete;
        ~BasicLogger() = delete;
        BasicLogger(const BasicLogger&) = delete;
        BasicLogger(BasicLogger&&) noexcept = delete;
        BasicLogger& operator=(const BasicLogger&) = delete;
        BasicLogger& operator=(BasicLogger&&) noexcept = delete;

        template <traits::DefinitionLogger Logger, typename... Args>
        static void PrintConsole(const log::Formater<Logger> format, [[maybe_unused]] Args&&... args);
    };
}

#endif // RENDU_TYPES_BASIC_LOGGER_H_
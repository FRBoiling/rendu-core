//
// Created by 沸腾 on 2025/11/22.
//
#pragma once

#ifndef RENDU_APPENDER_DEFAULT_H
#define RENDU_APPENDER_DEFAULT_H

#include "appender.h"
#include <vector>

#include "common/utils/enum_utils.h"

BEGIN_NAMESPACE_COMMON
    // EnumUtils: DESCRIBE THIS
    enum class ColorTypes : uint8
    {
        BLACK = 0,
        RED = 1,
        GREEN = 2,
        BROWN = 3,
        BLUE = 4,
        MAGENTA = 5,
        CYAN = 6,
        GREY = 7,
        YELLOW = 8,
        LRED = 9,
        LGREEN = 10,
        LBLUE = 11,
        LMAGENTA = 12,
        LCYAN = 13,
        WHITE = 14,
        NUM_COLOR_TYPES  // SKIP
    };

    class RC_COMMON_API AppenderConsole : public Appender
    {
    public:
        static constexpr AppenderType type = AppenderType::APPENDER_CONSOLE;

        AppenderConsole(uint8 id, std::string name, LogLevel level, AppenderFlags flags, std::vector<std::string_view> const& args);

        void initColors(std::string const& name, std::string_view init_str);
        [[nodiscard]] AppenderType getType() const override { return type; }

    private:
        void setColor(bool stdout_stream, ColorTypes color);
        void resetColor(bool stdout_stream);
        void print(std::string const& prefix, std::string const& text, bool error);
        void _write(LogMessage const* message) override;
        bool m_colored;
        ColorTypes m_colors[Utils::EnumToInt(LogLevel::NUM_ENABLED_LOG_LEVELS)]{};
    };

END_NAMESPACE_COMMON


#endif //RENDU_APPENDER_DEFAULT_H

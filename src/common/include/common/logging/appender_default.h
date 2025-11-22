//
// Created by 沸腾 on 2025/11/22.
//

#ifndef RENDU_APPENDER_DEFAULT_H
#define RENDU_APPENDER_DEFAULT_H

#include "appender.h"
#include <vector>

BEGIN_NAMESPACE_COMMON
    namespace Logging
    {
        // EnumUtils: DESCRIBE THIS
        enum ColorTypes
        {
            BLACK,
            RED,
            GREEN,
            BROWN,
            BLUE,
            MAGENTA,
            CYAN,
            GREY,
            YELLOW,
            LRED,
            LGREEN,
            LBLUE,
            LMAGENTA,
            LCYAN,
            WHITE,
            NUM_COLOR_TYPES // SKIP
        };

        class RC_COMMON_API AppenderDefault : public Appender
        {
        public:
            static constexpr AppenderType type = APPENDER_CONSOLE;

            AppenderDefault(uint8 _id, std::string name, LogLevel level, AppenderFlags flags,
                            std::vector<std::string_view> const& args);
            void InitColors(std::string const& name, std::string_view init_str);
            AppenderType getType() const override { return type; }

        private:
            void SetColor(bool stdout_stream, ColorTypes color);
            void ResetColor(bool stdout_stream);
            void Print(std::string const& prefix, std::string const& text, bool error);
            void _write(LogMessage const* message) override;
            bool _colored;
            ColorTypes _colors[NUM_ENABLED_LOG_LEVELS];
        };
    } // namespace Logging

END_NAMESPACE_COMMON


#endif //RENDU_APPENDER_DEFAULT_H

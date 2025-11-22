//
// Created by 沸腾 on 2025/11/22.
//

#include "common/logging/appender_default.h"

#include "common/logging/invalid_appender_args_exception.h"
#include "common/logging/log_message.h"
#include "common/utils/enum_utils.h"
#include "common/utils/optional.h"
#include "common/utils/string_convert.h"
#include "common/utils/string_format.h"
#include "common/utils/string_utils.h"

BEGIN_NAMESPACE_COMMON
    using namespace Logging;

#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
  #include <Windows.h>
#endif

AppenderDefault::AppenderDefault(uint8 id, std::string name, LogLevel level, AppenderFlags flags, std::vector<std::string_view> const& args)
    : Appender(id, std::move(name), level, flags), _colored(false)
{
    std::ranges::fill(_colors, NUM_COLOR_TYPES);

    if (args.size() > 3)
        InitColors(getName(), args[3]);
}

void AppenderDefault::InitColors(std::string const& name, std::string_view str)
{
    if (str.empty())
    {
        _colored = false;
        return;
    }

    std::vector<std::string_view> colorStrs = Utils::Tokenize(str, ' ', false);
    if (colorStrs.size() != NUM_ENABLED_LOG_LEVELS)
    {
        throw InvalidAppenderArgsException(Utils::StringFormat("Log::CreateAppenderFromConfig: Invalid color data '{}' for console appender {} (expected {} entries, got {})",
            str, name, Utils::EnumUtils::ToTitle(NUM_ENABLED_LOG_LEVELS), colorStrs.size()));
    }

    for (uint8 i = 0; i < NUM_ENABLED_LOG_LEVELS; ++i)
    {
        if (Optional<uint8> color = StringTo<uint8>(colorStrs[i]); color && Utils::EnumUtils::IsValid<ColorTypes>(*color))
            _colors[i] = static_cast<ColorTypes>(*color);
        else
        {
            throw InvalidAppenderArgsException(Utils::StringFormat("Log::CreateAppenderFromConfig: Invalid color '{}' for log level {} on console appender {}",
                colorStrs[i], Utils::EnumUtils::ToTitle(static_cast<LogLevel>(i)), name));
        }
    }

    _colored = true;
}

void AppenderDefault::SetColor(bool stdout_stream, ColorTypes color)
{
#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
    static WORD WinColorFG[NUM_COLOR_TYPES] =
    {
        0,                                                  // BLACK
        FOREGROUND_RED,                                     // RED
        FOREGROUND_GREEN,                                   // GREEN
        FOREGROUND_RED | FOREGROUND_GREEN,                  // BROWN
        FOREGROUND_BLUE,                                    // BLUE
        FOREGROUND_RED |                    FOREGROUND_BLUE, // MAGENTA
        FOREGROUND_GREEN | FOREGROUND_BLUE,                 // CYAN
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, // WHITE
                                                            // YELLOW
        FOREGROUND_RED | FOREGROUND_GREEN |                   FOREGROUND_INTENSITY,
                                                            // RED_BOLD
        FOREGROUND_RED |                                      FOREGROUND_INTENSITY,
                                                            // GREEN_BOLD
        FOREGROUND_GREEN |                   FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,             // BLUE_BOLD
                                                            // MAGENTA_BOLD
        FOREGROUND_RED |                    FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                                                            // CYAN_BOLD
        FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                                                            // WHITE_BOLD
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
    };

    HANDLE hConsole = GetStdHandle(stdout_stream ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    SetConsoleTextAttribute(hConsole, WinColorFG[color]);
#else
    enum ANSITextAttr
    {
        TA_NORMAL                                = 0,
        TA_BOLD                                  = 1,
        TA_BLINK                                 = 5,
        TA_REVERSE                               = 7
    };

    enum ANSIFgTextAttr
    {
        FG_BLACK                                 = 30,
        FG_RED,
        FG_GREEN,
        FG_BROWN,
        FG_BLUE,
        FG_MAGENTA,
        FG_CYAN,
        FG_WHITE,
        FG_YELLOW
    };

    enum ANSIBgTextAttr
    {
        BG_BLACK                                 = 40,
        BG_RED,
        BG_GREEN,
        BG_BROWN,
        BG_BLUE,
        BG_MAGENTA,
        BG_CYAN,
        BG_WHITE
    };

    static uint8 UnixColorFG[NUM_COLOR_TYPES] =
    {
        FG_BLACK,                                          // BLACK
        FG_RED,                                            // RED
        FG_GREEN,                                          // GREEN
        FG_BROWN,                                          // BROWN
        FG_BLUE,                                           // BLUE
        FG_MAGENTA,                                        // MAGENTA
        FG_CYAN,                                           // CYAN
        FG_WHITE,                                          // WHITE
        FG_YELLOW,                                         // YELLOW
        FG_RED,                                            // LRED
        FG_GREEN,                                          // LGREEN
        FG_BLUE,                                           // LBLUE
        FG_MAGENTA,                                        // LMAGENTA
        FG_CYAN,                                           // LCYAN
        FG_WHITE                                           // LWHITE
    };

    fprintf((stdout_stream? stdout : stderr), "\x1b[%d%sm", UnixColorFG[color], (color >= YELLOW && color < NUM_COLOR_TYPES ? ";1" : ""));
    #endif
}

void AppenderDefault::ResetColor(bool stdout_stream)
{
#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
    HANDLE hConsole = GetStdHandle(stdout_stream ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#else
    fputs("\x1b[0m", stdout_stream ? stdout : stderr);
#endif
}

void AppenderDefault::Print(std::string const& prefix, std::string const& text, bool error)
{
#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
    WriteWinConsole(prefix + text + "\n", error);
#else
    FILE* out = error ? stderr : stdout;
    fwrite(prefix.c_str(), 1, prefix.length(), out);
    fwrite(text.c_str(), 1, text.length(), out);
    fwrite("\n", 1, 1, out);
#endif
}

void AppenderDefault::_write(LogMessage const* message)
{
    bool stdout_stream = !(message->level == LOG_LEVEL_ERROR || message->level == LOG_LEVEL_FATAL);

    if (_colored)
    {
        uint8 index;
        switch (message->level)
        {
            case LOG_LEVEL_TRACE:
               index = 5;
               break;
            case LOG_LEVEL_DEBUG:
               index = 4;
               break;
            case LOG_LEVEL_INFO:
               index = 3;
               break;
            case LOG_LEVEL_WARN:
               index = 2;
               break;
            case LOG_LEVEL_FATAL:
               index = 0;
               break;
            case LOG_LEVEL_ERROR:
                [[fallthrough]];
            default:
               index = 1;
               break;
        }

        SetColor(stdout_stream, _colors[index]);
        Print(message->prefix, message->text, !stdout_stream);
        ResetColor(stdout_stream);
    }
    else
        Print(message->prefix, message->text, !stdout_stream);
}

END_NAMESPACE_COMMON
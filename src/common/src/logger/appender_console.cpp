//
// Created by 沸腾 on 2025/11/22.
//

#include "common/logger/appender_console.h"
#include "common/logger/log_message.h"
#include "common/asio/io_context.h"
#include "common/asio/executor_work_guard.h"
#include "common/asio/post.h"
#include "common/utils/enum_utils.h"
#include "common/utils/optional.h"
#include "common/utils/string_convert.h"
#include "common/utils/string_format.h"
#include "common/utils/string_utils.h"

BEGIN_NAMESPACE_COMMON
#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
#include <Windows.h>
#endif

    AppenderConsole::AppenderConsole(uint8 id, std::string name, LogLevel level, AppenderFlags flags,
                                     std::vector<std::string_view> const& args)
        : Appender(id, std::move(name), level, flags)
        , m_colored(false)
        , m_async(static_cast<bool>(flags & AppenderFlags::APPENDER_FLAGS_ASYNC))
        , m_ioContext(nullptr)
    {
        std::ranges::fill(m_colors, ColorTypes::NUM_COLOR_TYPES);

        if (args.size() > 3)
            initColors(getName(), args[3]);
    }

    AppenderConsole::~AppenderConsole()
    {
        flush();
    }

    void AppenderConsole::setIoContext(Asio::IoContext* ioContext)
    {
        if (!m_async)
        {
            return; // 同步模式不需要 IoContext
        }

        if (!ioContext)
        {
            throw std::invalid_argument("IoContext cannot be null for async mode");
        }

        m_ioContext = ioContext;

        // 创建工作守卫，防止 ioContext 空闲退出
        m_workGuard = std::make_unique<Asio::ExecutorWorkGuard>(*m_ioContext);
    }

    void AppenderConsole::setBatchSize(size_t batchSize)
    {
        m_batchSize = batchSize > 0 ? batchSize : 50;
    }

    void AppenderConsole::flush()
    {
        if (!m_async)
        {
            return;
        }

        // 异步模式：刷新所有待处理消息
        std::lock_guard<std::mutex> lock(m_queueMutex);

        while (!m_messageQueue.empty())
        {
            const ConsoleMessage& msg = m_messageQueue.front();

            if (m_colored)
            {
                setColor(msg.stdout_stream, msg.color);
                print(msg.prefix, msg.text, !msg.stdout_stream);
                resetColor(msg.stdout_stream);
            }
            else
            {
                print(msg.prefix, msg.text, !msg.stdout_stream);
            }

            m_messageQueue.pop_front();
        }

        m_pendingCount = 0;
    }

    size_t AppenderConsole::getPendingCount() const
    {
        return m_pendingCount;
    }

    void AppenderConsole::initColors(std::string const& name, std::string_view str)
    {
        if (str.empty())
        {
            m_colored = false;
            return;
        }

        std::vector<std::string_view> colorStrs = Utils::Tokenize(str, ' ', false);
        if (colorStrs.size() != static_cast<int>(LogLevel::NUM_ENABLED_LOG_LEVELS))
        {
            throw InvalidAppenderArgsException(Utils::StringFormat(
                "Log::CreateAppenderFromConfig: Invalid color data '{}' for console appender {} (expected {} entries, got {})",
                str, name, Utils::EnumUtils::ToTitle(LogLevel::NUM_ENABLED_LOG_LEVELS), colorStrs.size()));
        }

        for (int i = 0; i < static_cast<int>(LogLevel::NUM_ENABLED_LOG_LEVELS); ++i)
        {
            if (Optional<uint8> color = Utils::StringTo<uint8>(colorStrs[i]); color && Utils::EnumUtils::IsValid<
                ColorTypes>(*color))
                m_colors[i] = static_cast<ColorTypes>(*color);
            else
            {
                throw InvalidAppenderArgsException(Utils::StringFormat(
                    "Log::CreateAppenderFromConfig: Invalid color '{}' for log level {} on console appender {}",
                    colorStrs[i], Utils::EnumUtils::ToTitle(static_cast<LogLevel>(i)), name));
            }
        }

        m_colored = true;
    }

    void AppenderConsole::setColor(bool stdout_stream, ColorTypes color)
    {
#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
        static WORD WinColorFG[NUM_COLOR_TYPES] =
        {
            0, // BLACK
            FOREGROUND_RED, // RED
            FOREGROUND_GREEN, // GREEN
            FOREGROUND_RED | FOREGROUND_GREEN, // BROWN
            FOREGROUND_BLUE, // BLUE
            FOREGROUND_RED | FOREGROUND_BLUE, // MAGENTA
            FOREGROUND_GREEN | FOREGROUND_BLUE, // CYAN
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, // WHITE
            // YELLOW
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
            // RED_BOLD
            FOREGROUND_RED | FOREGROUND_INTENSITY,
            // GREEN_BOLD
            FOREGROUND_GREEN | FOREGROUND_INTENSITY,
            FOREGROUND_BLUE | FOREGROUND_INTENSITY, // BLUE_BOLD
            // MAGENTA_BOLD
            FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
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
            TA_NORMAL = 0,
            TA_BOLD = 1,
            TA_BLINK = 5,
            TA_REVERSE = 7
        };

        enum ANSIFgTextAttr
        {
            FG_BLACK = 30,
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
            BG_BLACK = 40,
            BG_RED,
            BG_GREEN,
            BG_BROWN,
            BG_BLUE,
            BG_MAGENTA,
            BG_CYAN,
            BG_WHITE
        };

        static uint8 UnixColorFG[static_cast<int>(ColorTypes::NUM_COLOR_TYPES)] =
        {
            FG_BLACK, // BLACK
            FG_RED, // RED
            FG_GREEN, // GREEN
            FG_BROWN, // BROWN
            FG_BLUE, // BLUE
            FG_MAGENTA, // MAGENTA
            FG_CYAN, // CYAN
            FG_WHITE, // WHITE
            FG_YELLOW, // YELLOW
            FG_RED, // LRED
            FG_GREEN, // LGREEN
            FG_BLUE, // LBLUE
            FG_MAGENTA, // LMAGENTA
            FG_CYAN, // LCYAN
            FG_WHITE // LWHITE
        };

        fprintf((stdout_stream ? stdout : stderr), "\x1b[%d%sm", UnixColorFG[static_cast<int>(color)],
                (color >= ColorTypes::YELLOW && color < ColorTypes::NUM_COLOR_TYPES ? ";1" : ""));
#endif
    }

    void AppenderConsole::resetColor(bool stdout_stream)
    {
#if RENDU_PLATFORM == RENDU_PLATFORM_WINDOWS
        HANDLE hConsole = GetStdHandle(stdout_stream ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#else
        fputs("\x1b[0m", stdout_stream ? stdout : stderr);
#endif
    }

    void AppenderConsole::print(std::string const& prefix, std::string const& text, bool error)
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

    void AppenderConsole::_write(LogMessage const* message)
    {
        bool stdout_stream = !(message->m_level == LogLevel::LOG_LEVEL_ERROR || message->m_level ==
            LogLevel::LOG_LEVEL_FATAL);

        ColorTypes color;
        if (m_colored)
        {
            uint8 index;
            switch (message->m_level)
            {
            case LogLevel::LOG_LEVEL_TRACE:
                index = 5;
                break;
            case LogLevel::LOG_LEVEL_DEBUG:
                index = 4;
                break;
            case LogLevel::LOG_LEVEL_INFO:
                index = 3;
                break;
            case LogLevel::LOG_LEVEL_WARN:
                index = 2;
                break;
            case LogLevel::LOG_LEVEL_FATAL:
                index = 0;
                break;
            case LogLevel::LOG_LEVEL_ERROR:
                [[fallthrough]];
            default:
                index = 1;
                break;
            }
            color = m_colors[index];
        }

        if (m_async)
        {
            _writeAsync(stdout_stream, color, message->m_prefix, message->m_text);
        }
        else
        {
            _writeSync(stdout_stream, color, message->m_prefix, message->m_text);
        }
    }

    void AppenderConsole::_writeSync(bool stdout_stream, ColorTypes color, const std::string& prefix, const std::string& text)
    {
        if (m_colored)
        {
            setColor(stdout_stream, color);
            print(prefix, text, !stdout_stream);
            resetColor(stdout_stream);
        }
        else
        {
            print(prefix, text, !stdout_stream);
        }
    }

    void AppenderConsole::_writeAsync(bool stdout_stream, ColorTypes color, const std::string& prefix, const std::string& text)
    {
        if (!m_ioContext)
        {
            throw std::runtime_error("IoContext not set for async mode");
        }

        // 将消息加入队列
        ConsoleMessage msg(stdout_stream, color, prefix, text);

        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_messageQueue.push_back(std::move(msg));
            ++m_pendingCount;
        }

        // 检查是否需要批量刷新
        if (m_pendingCount >= m_batchSize && !m_flushing.exchange(true))
        {
            // 提交异步刷新任务
            Asio::Post(*m_ioContext, [this]()
            {
                _asyncFlush();
                m_flushing = false;
            });
        }
    }

    void AppenderConsole::_asyncFlush()
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);

        while (!m_messageQueue.empty())
        {
            const ConsoleMessage& msg = m_messageQueue.front();

            if (m_colored)
            {
                setColor(msg.stdout_stream, msg.color);
                print(msg.prefix, msg.text, !msg.stdout_stream);
                resetColor(msg.stdout_stream);
            }
            else
            {
                print(msg.prefix, msg.text, !msg.stdout_stream);
            }

            m_messageQueue.pop_front();
        }

        m_pendingCount = 0;
    }

END_NAMESPACE_COMMON

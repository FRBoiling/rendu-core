#pragma once

#include "common/define.h"
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <map>

BEGIN_NAMESPACE_COMMON
    namespace log
    {
        enum class Level : uint8_t
        {
            Trace = 0,
            Debug,
            Info,
            Warn,
            Error,
            Critical
        };

        struct LogMessage
        {
            Level level;
            std::string logger_name;
            std::string message;
            std::string timestamp;
            std::thread::id thread_id;
            std::map<std::string, std::string> fields; // 结构化字段
        };

        class Formatter
        {
        public:
            Formatter() = default;
            virtual ~Formatter() = default;

            virtual std::string format(const LogMessage& msg) = 0;
        };

        class DefaultFormatter : public Formatter
        {
        public:
            DefaultFormatter() = default;
            ~DefaultFormatter() override = default;

            std::string format(const LogMessage& msg) override;

        private:
            std::string level_to_string(Level level);
        };

        // 模式格式化器，支持自定义格式
        // 支持的占位符:
        // %t - timestamp
        // %l - level
        // %n - logger name
        // %m - message
        // %i - thread id
        // %f - fields (结构化字段)
        // %% - %
        class PatternFormatter : public Formatter
        {
        public:
            explicit PatternFormatter(const std::string& pattern = "[%t] [%l] [%n] %m");
            ~PatternFormatter() override = default;

            std::string format(const LogMessage& msg) override;

        private:
            std::string level_to_string(Level level);
            std::string thread_id_to_string(std::thread::id id);

            std::string pattern_;
        };
    } // namespace log
END_NAMESPACE_COMMON

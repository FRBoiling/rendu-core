//
// Created by 沸腾 on 2025/11/22.
//
#pragma once

#ifndef RENDU_APPENDER_DEFAULT_H
#define RENDU_APPENDER_DEFAULT_H

#include "appender.h"
#include "common/asio/io_context.h"
#include <vector>
#include <deque>
#include <mutex>
#include <atomic>

#include "common/utils/enum_utils.h"

BEGIN_NAMESPACE_COMMON

    // 前置声明
    struct LogMessage;

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

    // 控制台日志消息结构
    struct ConsoleMessage
    {
        bool stdout_stream;
        ColorTypes color;
        std::string prefix;
        std::string text;

        ConsoleMessage(bool stdout_, ColorTypes color_, std::string prefix_, std::string text_)
            : stdout_stream(stdout_)
            , color(color_)
            , prefix(std::move(prefix_))
            , text(std::move(text_))
        {}
    };

    class RC_COMMON_API AppenderConsole : public Appender
    {
    public:
        static constexpr AppenderType type = AppenderType::APPENDER_CONSOLE;

        AppenderConsole(uint8 id, std::string name, LogLevel level, AppenderFlags flags, std::vector<std::string_view> const& args);

        ~AppenderConsole() override;

        /**
         * @brief 设置 IoContext（异步模式必须调用）
         * @param ioContext IoContext 指针
         */
        void setIoContext(Asio::IoContext* ioContext);

        /**
         * @brief 设置批量大小（异步模式）
         * @param batchSize 批量大小
         */
        void setBatchSize(size_t batchSize);

        /**
         * @brief 强制刷新缓冲区
         */
        void flush();

        /**
         * @brief 获取待处理消息数量（异步模式）
         */
        [[nodiscard]] size_t getPendingCount() const;

        void initColors(std::string const& name, std::string_view init_str);
        [[nodiscard]] AppenderType getType() const override { return type; }

    private:
        void setColor(bool stdout_stream, ColorTypes color);
        void resetColor(bool stdout_stream);
        void print(std::string const& prefix, std::string const& text, bool error);
        void _write(LogMessage const* message) override;

        /**
         * @brief 同步写入
         */
        void _writeSync(bool stdout_stream, ColorTypes color, const std::string& prefix, const std::string& text);

        /**
         * @brief 异步写入（提交到队列）
         */
        void _writeAsync(bool stdout_stream, ColorTypes color, const std::string& prefix, const std::string& text);

        /**
         * @brief 异步批量刷新处理
         */
        void _asyncFlush();

        // 成员变量
        bool m_colored;
        ColorTypes m_colors[Utils::EnumToInt(LogLevel::NUM_ENABLED_LOG_LEVELS)]{};

        // 异步模式相关
        bool m_async;                                   ///< 是否异步模式
        Asio::IoContext* m_ioContext;                   ///< IoContext（异步模式）
        std::unique_ptr<Asio::ExecutorWorkGuard> m_workGuard; ///< 工作守卫
        std::deque<ConsoleMessage> m_messageQueue;       ///< 消息队列
        mutable std::mutex m_queueMutex;                ///< 队列互斥锁
        size_t m_batchSize = 50;                         ///< 批量大小
        std::atomic<size_t> m_pendingCount{0};           ///< 待处理计数
        std::atomic<bool> m_flushing{false};             ///< 正在刷新
    };

END_NAMESPACE_COMMON


#endif //RENDU_APPENDER_DEFAULT_H

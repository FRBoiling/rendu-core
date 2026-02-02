#pragma once

#include "common/define.h"
#include <functional>
#include <memory>
#include <atomic>
#include <chrono>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/deadline_timer.hpp>

BEGIN_NAMESPACE_COMMON
    namespace io
    {
        class IoContext;

        /**
         * @brief 定时器类
         *
         * 提供一次性定时器和周期性定时器功能
         */
        class Timer
        {
        public:
            using Callback = std::function<void()>;
            using CancelToken = std::shared_ptr<std::atomic<bool>>;

            /**
             * @brief 构造函数
             * @param io I/O 上下文
             */
            explicit Timer(IoContext& io);
            ~Timer();

            // 禁止拷贝
            Timer(const Timer&) = delete;
            Timer& operator=(const Timer&) = delete;
            Timer(Timer&&) = default;
            Timer& operator=(Timer&&) = default;

            /**
             * @brief 设置一次性定时器（延迟执行）
             * @param delay 延迟时间
             * @param callback 回调函数
             */
            void expires_after(std::chrono::milliseconds delay, Callback callback);

            /**
             * @brief 设置一次性定时器（在指定时间点执行）
             * @param time 时间点
             * @param callback 回调函数
             */
            void expires_at(std::chrono::steady_clock::time_point time, Callback callback);

            /**
             * @brief 设置周期性定时器
             * @param interval 间隔时间
             * @param callback 回调函数
             * @return 取消令牌，可用于停止定时器
             */
            CancelToken repeat(std::chrono::milliseconds interval, Callback callback);

            /**
             * @brief 取消定时器
             */
            void cancel();

            /**
             * @brief 检查定时器是否激活
             * @return 是否激活
             */
            bool active() const;

        private:
            IoContext& io_;
            std::unique_ptr<boost::asio::steady_timer> timer_;
            CancelToken cancel_token_;
            std::atomic<bool> active_;
        };
    } // namespace io
END_NAMESPACE_COMMON

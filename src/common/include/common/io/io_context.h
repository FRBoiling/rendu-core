#pragma once

#include "common/define.h"
#include <memory>
#include <functional>
#include <thread>
#include <vector>
#include <atomic>
#include <boost/asio/io_context.hpp>

BEGIN_NAMESPACE_COMMON
    namespace io
    {
        /**
         * @brief I/O 上下文封装
         *
         * 封装 boost::asio::io_context，提供线程池支持
         */
        class IoContext
        {
        public:
            /**
             * @brief 构造函数
             * @param thread_count 线程池大小，默认为 1
             */
            explicit IoContext(size_t thread_count = 1);
            ~IoContext();

            // 禁止拷贝和移动
            IoContext(const IoContext&) = delete;
            IoContext& operator=(const IoContext&) = delete;
            IoContext(IoContext&&) = delete;
            IoContext& operator=(IoContext&&) = delete;

            /**
             * @brief 提交任务到事件循环
             * @param task 要执行的任务
             */
            void post(std::function<void()> task);

            /**
             * @brief 运行事件循环（阻塞）
             */
            void run();

            /**
             * @brief 停止事件循环
             */
            void stop();

            /**
             * @brief 获取底层 io_context
             * @return io_context 引用
             */
            boost::asio::io_context& native();
            const boost::asio::io_context& native() const;

            /**
             * @brief 检查是否运行中
             * @return 是否运行中
             */
            bool running() const;

        private:
            boost::asio::io_context io_context_;
            std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work_;
            std::vector<std::thread> threads_;
            std::atomic<bool> running_;
            size_t thread_count_;
        };
    } // namespace io
END_NAMESPACE_COMMON

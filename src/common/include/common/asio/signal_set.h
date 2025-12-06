//
// Created by 沸腾 on 2025/11/14.
//

#ifndef RENDU_SIGNAL_SET_H
#define RENDU_SIGNAL_SET_H

#include "common/define.h"
#include <memory>
#include <functional>

BEGIN_NAMESPACE_COMMON
    namespace Asio
    {
        class IoContext;

        class SignalSet
        {
        public:
            explicit SignalSet(IoContext& ioContext);
            
            SignalSet(IoContext& ioContext, int signal_number);
            
            SignalSet(IoContext& ioContext, int signal_number1, int signal_number2);
            
            SignalSet(IoContext& ioContext, int signal_number1, int signal_number2, int signal_number3);

            ~SignalSet();

            SignalSet(const SignalSet&) = delete;
            SignalSet& operator=(const SignalSet&) = delete;
            SignalSet(SignalSet&&) noexcept;
            SignalSet& operator=(SignalSet&&) noexcept;

            // 添加信号
            void add(int signal_number);

            // 移除信号
            void remove(int signal_number);

            // 清空所有信号
            void clear();

            // 取消所有异步操作
            void cancel();

            // 异步等待信号 (仅信号号)
            void async_wait(std::function<void(int)> handler);

            // 异步等待信号 (带错误码)
            void async_wait(std::function<void(const std::error_code&, int)> handler);

        private:
            friend class IoContext;
            class Impl;
            std::unique_ptr<Impl> pImpl_;
        };
    } // namespace Asio

END_NAMESPACE_COMMON

#endif //RENDU_SIGNAL_SET_H
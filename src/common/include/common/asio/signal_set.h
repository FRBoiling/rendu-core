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
            explicit SignalSet(const IoContext& ioContext);
            
            SignalSet(const IoContext& ioContext, int signalNumber);
            
            SignalSet(const IoContext& ioContext, int signalNumber1, int signalNumber2);

            SignalSet(const IoContext& ioContext, int signalNumber1, int signalNumber2, int signalNumber3);

            ~SignalSet();

            SignalSet(const SignalSet&) = delete;
            SignalSet& operator=(const SignalSet&) = delete;
            SignalSet(SignalSet&&) noexcept;
            SignalSet& operator=(SignalSet&&) noexcept;

            // 添加信号
            void add(int signalNumber) const;

            // 移除信号
            void remove(int signalNumber) const;

            // 清空所有信号
            void clear() const;

            // 取消所有异步操作
            void cancel() const;

            // 异步等待信号 (仅信号号)
            void asyncWait(std::function<void(int)> handler) const;

            // 异步等待信号 (带错误码)
            void asyncWait(std::function<void(const std::error_code&, int)> handler) const;

        private:
            friend class IoContext;
            class Impl;
            std::unique_ptr<Impl> m_pImpl;
        };
    } // namespace Asio

END_NAMESPACE_COMMON

#endif //RENDU_SIGNAL_SET_H
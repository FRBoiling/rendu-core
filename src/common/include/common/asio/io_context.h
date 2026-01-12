//
// Created by 沸腾 on 2025/11/13.
//

#ifndef RENDU_IO_CONTEXT_H
#define RENDU_IO_CONTEXT_H

#include <memory>
#include <functional>

#include "common/define.h"


BEGIN_NAMESPACE_COMMON
    namespace Asio
    {
        class ExecutorWorkGuard;
        class SignalSet;

        class IoContext
        {
        public:
            IoContext();
            explicit IoContext(int concurrencyHint);
            ~IoContext();

            IoContext(const IoContext&) = delete;
            IoContext& operator=(const IoContext&) = delete;
            IoContext(IoContext&&) noexcept;
            IoContext& operator=(IoContext&&) noexcept;

            [[nodiscard]] std::size_t run() const;
            [[nodiscard]] std::size_t poll() const;
            void stop() const;
            [[nodiscard]] bool stopped() const;
            void restart() const;

            class Strand;
            class Executor;

            void post(const std::function<void()>& handler) const;

        private:
            friend class ExecutorWorkGuard;
            friend class SignalSet;

            class Impl;
            std::unique_ptr<Impl> m_pImpl;
        };


    } // namespace Asio

END_NAMESPACE_COMMON

#endif //RENDU_IO_CONTEXT_H
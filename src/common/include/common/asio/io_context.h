//
// Created by 沸腾 on 2025/11/13.
//

#ifndef RENDU_IO_CONTEXT_H
#define RENDU_IO_CONTEXT_H

#include <memory>
#include <functional>

#include "executor.h"
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
            explicit IoContext(int concurrency_hint);
            ~IoContext();

            IoContext(const IoContext&) = delete;
            IoContext& operator=(const IoContext&) = delete;
            IoContext(IoContext&&) noexcept;
            IoContext& operator=(IoContext&&) noexcept;

            std::size_t run();
            std::size_t poll();
            void stop();
            bool stopped() const;
            void restart();

            class Strand;
            class Executor;

            Strand make_strand();
            void post(std::function<void()> handler) const;

            Executor get_executor();

        private:
            friend class ExecutorWorkGuard;
            friend class SignalSet;

            class Impl;
            std::unique_ptr<Impl> pImpl_;
        };


    } // namespace Asio

END_NAMESPACE_COMMON

#endif //RENDU_IO_CONTEXT_H
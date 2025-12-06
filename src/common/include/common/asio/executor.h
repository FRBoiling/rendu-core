//
// Created by 沸腾 on 2025/12/23.
//

#ifndef RENDU_EXECUTOR_H
#define RENDU_EXECUTOR_H

#include <functional>
#include <memory>
#include "common/define.h"

BEGIN_NAMESPACE_COMMON
    namespace Asio
    {
        class Executor
        {
        public:
            Executor();
            ~Executor();

            Executor(const Executor&) = delete;
            Executor& operator=(const Executor&) = delete;
            Executor(Executor&&) noexcept;
            Executor& operator=(Executor&&) noexcept;

            void post(std::function<void()> handler);

        private:
            friend class IoContext;
            class Impl;
            std::unique_ptr<Impl> pImpl_;

            explicit Executor(std::unique_ptr<Impl> impl);
        };
    } // namespace Asio

END_NAMESPACE_COMMON

#endif //RENDU_EXECUTOR_H
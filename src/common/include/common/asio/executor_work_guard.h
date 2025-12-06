//
// Created by 沸腾 on 2025/12/22.
//

#ifndef RENDU_WORK_H
#define RENDU_WORK_H

#include "common/define.h"
#include <memory>

BEGIN_NAMESPACE_COMMON
    namespace Asio
    {
        class IoContext;

        class ExecutorWorkGuard
        {
        public:
            explicit ExecutorWorkGuard(IoContext& io_context);
            ~ExecutorWorkGuard();

            // 禁用复制构造和赋值
            ExecutorWorkGuard(const ExecutorWorkGuard&) = delete;
            ExecutorWorkGuard& operator=(const ExecutorWorkGuard&) = delete;

            // 允许移动构造和赋值
            ExecutorWorkGuard(ExecutorWorkGuard&&) noexcept;
            ExecutorWorkGuard& operator=(ExecutorWorkGuard&&) noexcept;

        private:
            friend class IoContext;
            class Impl;
            std::unique_ptr<Impl> pImpl_;
        };
    } // namespace Asio

END_NAMESPACE_COMMON

#endif //RENDU_WORK_H

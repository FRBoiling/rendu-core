//
// Created by 沸腾 on 2025/12/23.
//

// ============================================================================
// executor.h - 执行器封装
// ============================================================================

#ifndef RENDU_EXECUTOR_H
#define RENDU_EXECUTOR_H

#include <functional>
#include <memory>
#include "common/define.h"

BEGIN_NAMESPACE_COMMON

namespace Asio
{

// ============================================================================
// Executor - 执行器
// ============================================================================

class Executor
{
public:
    /**
     * @brief 构造函数
     */
    Executor();

    /**
     * @brief 析构函数
     */
    ~Executor();

    // 禁用复制
    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;

    // 支持移动
    Executor(Executor&&) noexcept;
    Executor& operator=(Executor&&) noexcept;

    /**
     * @brief 投递任务到执行器
     * @param handler 要执行的处理程序
     */
    void post(const std::function<void()>& handler) const;

private:
    // 友元类声明
    friend class IoContext;

    // PIMPL 实现
    class Impl;
    std::unique_ptr<Impl> m_pImpl;

    /**
     * @brief 私有构造函数，从实现对象创建
     * @param impl 实现对象
     */
    explicit Executor(std::unique_ptr<Impl> impl);
};

} // namespace Asio

END_NAMESPACE_COMMON

#endif // RENDU_EXECUTOR_H

//
// Created by 沸腾 on 2025/12/22.
//

// ============================================================================
// executor_work_guard.h - 执行器工作守护（防止 I/O 上下文退出）
// ============================================================================

#ifndef RENDU_WORK_H
#define RENDU_WORK_H

#include "common/define.h"
#include <memory>

BEGIN_NAMESPACE_COMMON

namespace Asio
{

// 前置声明
class IoContext;

// ============================================================================
// ExecutorWorkGuard - 执行器工作守护
// 用于防止 I/O 上下文在没有待处理任务时退出
// ============================================================================

class ExecutorWorkGuard
{
public:
    /**
     * @brief 构造函数，从 I/O 上下文创建工作守护
     * @param ioContext I/O 上下文引用
     */
    explicit ExecutorWorkGuard(const IoContext& ioContext);

    /**
     * @brief 析构函数，自动释放工作守护
     */
    ~ExecutorWorkGuard();

    // 禁用复制
    ExecutorWorkGuard(const ExecutorWorkGuard&) = delete;
    ExecutorWorkGuard& operator=(const ExecutorWorkGuard&) = delete;

    // 支持移动
    ExecutorWorkGuard(ExecutorWorkGuard&&) noexcept;
    ExecutorWorkGuard& operator=(ExecutorWorkGuard&&) noexcept;

private:
    // 友元类声明
    friend class IoContext;

    // PIMPL 实现
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};

} // namespace Asio

END_NAMESPACE_COMMON

#endif // RENDU_WORK_H

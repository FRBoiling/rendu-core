//
// Created by 沸腾 on 2025/11/14.
//

// ============================================================================
// io_context_strand.h - I/O 上下文串行执行器（保证任务顺序执行）
// ============================================================================

#ifndef RENDU_IO_CONTEXT_STRAND_H
#define RENDU_IO_CONTEXT_STRAND_H

#include <memory>
#include <functional>
#include "common/define.h"

BEGIN_NAMESPACE_COMMON

namespace Asio
{

// ============================================================================
// IoContext::Strand - 串行执行器
// 保证提交的任务按照提交的顺序执行，即使在多线程环境中
// ============================================================================

class IoContext::Strand
{
public:
    /**
     * @brief 构造函数，从 I/O 上下文创建串行执行器
     * @param ioContext I/O 上下文引用
     */
    Strand(const IoContext& ioContext);

    /**
     * @brief 析构函数
     */
    ~Strand();

    // 禁用复制
    Strand(const Strand&) = delete;
    Strand& operator=(const Strand&) = delete;

    // 支持移动
    Strand(Strand&&) noexcept;
    Strand& operator=(Strand&&) noexcept;

    /**
     * @brief 投递任务到串行执行器
     * @param handler 要执行的处理程序
     */
    void post(const std::function<void()>& handler) const;

    /**
     * @brief 调度任务到串行执行器
     * 如果在当前串行执行器上下文中调用，则立即执行
     * @param handler 要执行的处理程序
     */
    void dispatch(const std::function<void()>& handler) const;

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
    explicit Strand(std::unique_ptr<Impl> impl);
};

} // namespace Asio

END_NAMESPACE_COMMON

#endif // RENDU_IO_CONTEXT_STRAND_H

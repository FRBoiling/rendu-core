//
// Created by 沸腾 on 2025/11/13.
//

// ============================================================================
// io_context.h - 异步 I/O 上下文封装
// ============================================================================

#ifndef RENDU_IO_CONTEXT_H
#define RENDU_IO_CONTEXT_H

#include <memory>
#include <functional>

#include "common/define.h"


BEGIN_NAMESPACE_COMMON

namespace Asio
{

// 前置声明
class ExecutorWorkGuard;
class SignalSet;
class TcpSocket;
class TcpAcceptor;

// ============================================================================
// IoContext - 异步 I/O 上下文
// ============================================================================

class IoContext
{
public:
    /**
     * @brief 构造函数，使用默认并发提示
     */
    IoContext();

    /**
     * @brief 构造函数，指定并发提示
     * @param concurrencyHint 并发提示值
     */
    explicit IoContext(int concurrencyHint);

    /**
     * @brief 析构函数
     */
    ~IoContext();

    // 禁用复制
    IoContext(const IoContext&) = delete;
    IoContext& operator=(const IoContext&) = delete;

    // 支持移动
    IoContext(IoContext&&) noexcept;
    IoContext& operator=(IoContext&&) noexcept;

    /**
     * @brief 运行 I/O 上下文事件循环
     * @return 执行的处理程序数量
     */
    [[nodiscard]] std::size_t run() const;

    /**
     * @brief 轮询 I/O 上下文，不阻塞
     * @return 执行的处理程序数量
     */
    [[nodiscard]] std::size_t poll() const;

    /**
     * @brief 停止 I/O 上下文
     */
    void stop() const;

    /**
     * @brief 检查 I/O 上下文是否已停止
     * @return true 如果已停止，否则 false
     */
    [[nodiscard]] bool stopped() const;

    /**
     * @brief 重启已停止的 I/O 上下文
     */
    void restart() const;

    // 前置声明
    class Strand;
    class Executor;

    /**
     * @brief 投递任务到 I/O 上下文
     * @param handler 要执行的处理程序
     */
    void post(const std::function<void()>& handler) const;

    /**
     * @brief 获取原生 boost::asio::io_context 引用
     * @return 原生 io_context 引用
     */
    void* getNative() const;

private:
    // 友元类声明
    friend class ExecutorWorkGuard;
    friend class SignalSet;

    // PIMPL 实现
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};

} // namespace Asio

END_NAMESPACE_COMMON

#endif // RENDU_IO_CONTEXT_H

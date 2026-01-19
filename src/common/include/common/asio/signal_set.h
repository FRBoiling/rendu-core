//
// Created by 沸腾 on 2025/11/14.
//

// ============================================================================
// signal_set.h - 信号集合（用于处理系统信号）
// ============================================================================

#ifndef RENDU_SIGNAL_SET_H
#define RENDU_SIGNAL_SET_H

#include "common/define.h"
#include <memory>
#include <functional>
#include <system_error>

BEGIN_NAMESPACE_COMMON

namespace Asio
{

// 前置声明
class IoContext;

// ============================================================================
// SignalSet - 信号集合
// 用于异步处理系统信号（如 SIGINT, SIGTERM）
// ============================================================================

class SignalSet
{
public:
    /**
     * @brief 构造函数，创建空信号集
     * @param ioContext I/O 上下文引用
     */
    explicit SignalSet(const IoContext& ioContext);

    /**
     * @brief 构造函数，创建包含单个信号的信号集
     * @param ioContext I/O 上下文引用
     * @param signalNumber 信号编号
     */
    SignalSet(const IoContext& ioContext, int signalNumber);

    /**
     * @brief 构造函数，创建包含两个信号的信号集
     * @param ioContext I/O 上下文引用
     * @param signalNumber1 第一个信号编号
     * @param signalNumber2 第二个信号编号
     */
    SignalSet(const IoContext& ioContext, int signalNumber1, int signalNumber2);

    /**
     * @brief 构造函数，创建包含三个信号的信号集
     * @param ioContext I/O 上下文引用
     * @param signalNumber1 第一个信号编号
     * @param signalNumber2 第二个信号编号
     * @param signalNumber3 第三个信号编号
     */
    SignalSet(const IoContext& ioContext, int signalNumber1, int signalNumber2, int signalNumber3);

    /**
     * @brief 析构函数
     */
    ~SignalSet();

    // 禁用复制
    SignalSet(const SignalSet&) = delete;
    SignalSet& operator=(const SignalSet&) = delete;

    // 支持移动
    SignalSet(SignalSet&&) noexcept;
    SignalSet& operator=(SignalSet&&) noexcept;

    /**
     * @brief 添加信号到信号集
     * @param signalNumber 信号编号
     */
    void add(int signalNumber) const;

    /**
     * @brief 从信号集中移除信号
     * @param signalNumber 信号编号
     */
    void remove(int signalNumber) const;

    /**
     * @brief 清空信号集中的所有信号
     */
    void clear() const;

    /**
     * @brief 取消所有异步等待操作
     */
    void cancel() const;

    /**
     * @brief 异步等待信号（仅信号号）
     * @param handler 信号处理函数，接收信号编号
     */
    void asyncWait(std::function<void(int)> handler) const;

    /**
     * @brief 异步等待信号（带错误码）
     * @param handler 信号处理函数，接收错误码和信号编号
     */
    void asyncWait(std::function<void(const std::error_code&, int)> handler) const;

private:
    // 友元类声明
    friend class IoContext;

    // PIMPL 实现
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};

} // namespace Asio

END_NAMESPACE_COMMON

#endif // RENDU_SIGNAL_SET_H

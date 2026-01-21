//
// Created by 沸腾 on 2025/11/14.
//

// ============================================================================
// post.h - 任务投递工具函数
// ============================================================================

#ifndef RENDU_POST_H
#define RENDU_POST_H

#include "common/define.h"
#include "io_context.h"
#include "io_context_strand.h"

BEGIN_NAMESPACE_COMMON

namespace Asio
{

// ============================================================================
// 任务投递函数
// ============================================================================

/**
 * @brief 将任务投递到 I/O 上下文
 * @tparam T 任务类型
 * @param ioContext I/O 上下文引用
 * @param handler 要执行的任务
 */
template <typename T>
void Post(IoContext& ioContext, T&& handler)
{
    ioContext.post(std::forward<T>(handler));
}

/**
 * @brief 将任务投递到串行执行器
 * @tparam T 任务类型
 * @param strand 串行执行器引用
 * @param handler 要执行的任务
 */
template <typename T>
void Post(IoContext::Strand& strand, T&& handler)
{
    // 使用共享指针确保任务生命周期
    auto handler_ptr = std::make_shared<std::decay_t<T>>(std::forward<T>(handler));
    strand.post([handler_ptr]()
    {
        (*handler_ptr)();
    });
}

} // namespace Asio

END_NAMESPACE_COMMON

#endif // RENDU_POST_H

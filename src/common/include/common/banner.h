//
// Created by 沸腾 on 2025/10/24.
//

// ============================================================================
// banner.h - 应用启动横幅显示
// ============================================================================

#ifndef RENDU_BANNER_H
#define RENDU_BANNER_H

#include "common.h"
#include "define.h"

BEGIN_NAMESPACE_COMMON

namespace Banner
{
/**
 * @brief 显示应用启动横幅
 * @param applicationName 应用程序名称
 * @param log 普通日志输出回调函数
 * @param logExtraInfo 额外信息日志输出回调函数
 */
RC_COMMON_API void Show(
    char const* applicationName,
    void (*log)(char const* text),
    void (*logExtraInfo)(char const* text)
);
}

END_NAMESPACE_COMMON

#endif // RENDU_BANNER_H

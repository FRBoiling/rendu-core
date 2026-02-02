//
// Created by 沸腾 on 2026/2/2.
//

#ifndef RENDUCORE_CHECK_RESULT_H
#define RENDUCORE_CHECK_RESULT_H

#include "common/define.h"
#include <chrono>
#include <optional>
#include <string>

BEGIN_NAMESPACE_COMMON
    /**
       * @brief 健康检查结果
       */
    struct CheckResult
    {
        bool passed; ///< 检查是否通过
        std::string message; ///< 检查消息
        std::optional<std::chrono::milliseconds> duration; ///< 检查耗时

        CheckResult() : passed(true)
        {
        }

        CheckResult(bool p, const std::string& msg)
            : passed(p), message(msg)
        {
        }

        CheckResult(bool p, const std::string& msg,
                    std::chrono::milliseconds d)
            : passed(p), message(msg), duration(d)
        {
        }
    };

END_NAMESPACE_COMMON

#endif //RENDUCORE_CHECK_RESULT_H

//
// Created by 沸腾 on 2026/2/2.
//

#ifndef RENDUCORE_HEALTH_STATUS_H
#define RENDUCORE_HEALTH_STATUS_H

#include <string>
#include <map>
#include "common/define.h"

BEGIN_NAMESPACE_COMMON
    /**
      * @brief 健康检查状态
      */
    struct HealthStatus
    {
        bool healthy; ///< 整体健康状态
        std::string status; ///< 状态描述
        std::map<std::string, bool> checks; ///< 各检查项结果
        std::map<std::string, std::string> details; ///< 各检查项详细信息

        /**
         * @brief 转换为 JSON 格式
         */
        std::string to_json() const;

        /**
         * @brief 转换为纯文本格式
         */
        std::string to_text() const;
    };

END_NAMESPACE_COMMON

#endif //RENDUCORE_HEALTH_STATUS_H

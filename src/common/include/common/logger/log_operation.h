//
// Created by 沸腾 on 2025/11/22.
//

#pragma once

#ifndef RENDU_LOG_OPERATION_H
#define RENDU_LOG_OPERATION_H

#include "common/define.h"

#include <memory>

BEGIN_NAMESPACE_COMMON
        class Logger;
        struct LogMessage;

        class LogOperation
        {
        public:
            LogOperation(Logger const* logger, LogMessage* msg);
            LogOperation(LogOperation const&) = delete;
            LogOperation(LogOperation&&) noexcept = default;
            LogOperation& operator=(LogOperation const&) = delete;
            LogOperation& operator=(LogOperation&&) noexcept = default;
            ~LogOperation();

            void operator()() const;

        protected:
            Logger const* m_logger;
            std::unique_ptr<LogMessage> m_message;
        };

END_NAMESPACE_COMMON

#endif //RENDU_LOG_OPERATION_H

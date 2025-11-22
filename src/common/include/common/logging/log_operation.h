//
// Created by 沸腾 on 2025/11/22.
//

#ifndef RENDU_LOG_OPERATION_H
#define RENDU_LOG_OPERATION_H

#include "common/define.h"

#include <memory>

BEGIN_NAMESPACE_COMMON
    namespace Logging
    {
        class Logger;
        struct LogMessage;

        class LogOperation
        {
        public:
            LogOperation(Logger const* _logger, LogMessage* _msg);
            LogOperation(LogOperation const&) = delete;
            LogOperation(LogOperation&&) noexcept = default;
            LogOperation& operator=(LogOperation const&) = delete;
            LogOperation& operator=(LogOperation&&) noexcept = default;
            ~LogOperation();

            void operator()() const;

        protected:
            Logger const* logger;
            std::unique_ptr<LogMessage> msg;
        };
    } // namespace Logging

END_NAMESPACE_COMMON

#endif //RENDU_LOG_OPERATION_H

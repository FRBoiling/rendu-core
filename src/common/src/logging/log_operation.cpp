//
// Created by 沸腾 on 2025/11/22.
//

#include "common/logging/logger.h"
#include "common/logging/log_message.h"
#include "common/logging/log_operation.h"

BEGIN_NAMESPACE_COMMON
    using namespace Logging;

    LogOperation::LogOperation(Logger const* _logger, LogMessage* _msg) : logger(_logger), msg(_msg)
    {
    }

    LogOperation::~LogOperation() = default;

    void LogOperation::operator()() const
    {
        logger->write(msg.get());
    }

END_NAMESPACE_COMMON

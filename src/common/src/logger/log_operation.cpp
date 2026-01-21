//
// Created by 沸腾 on 2025/11/22.
//

#include "common/logger/logger.h"
#include "common/logger/log_message.h"
#include "common/logger/log_operation.h"

BEGIN_NAMESPACE_COMMON

    LogOperation::LogOperation(Logger const* logger, LogMessage* msg) : m_logger(logger), m_message(msg)
    {
    }

    LogOperation::~LogOperation() = default;

    void LogOperation::operator()() const
    {
        m_logger->write(m_message.get());
    }

END_NAMESPACE_COMMON

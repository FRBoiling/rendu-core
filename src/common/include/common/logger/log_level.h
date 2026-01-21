//
// Created by 沸腾 on 2025/10/31.
//
#pragma once

#ifndef RENDU_LOGLEVEL_H
#define RENDU_LOGLEVEL_H

#include "common/define.h"

BEGIN_NAMESPACE_COMMON

    enum class LogLevel : uint8
    {
        LOG_LEVEL_DISABLED = 0,
        LOG_LEVEL_TRACE = 1,
        LOG_LEVEL_DEBUG = 2,
        LOG_LEVEL_INFO = 3,
        LOG_LEVEL_WARN = 4,
        LOG_LEVEL_ERROR = 5,
        LOG_LEVEL_FATAL = 6,

        NUM_ENABLED_LOG_LEVELS = LOG_LEVEL_FATAL, // SKIP
        LOG_LEVEL_INVALID = 0xFF // SKIP
    };

END_NAMESPACE_COMMON

#endif //RENDU_LOGLEVEL_H

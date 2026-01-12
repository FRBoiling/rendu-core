//
// Created by 沸腾 on 2026/1/12.
//

#ifndef RENDU_APPENDER_TYPE_H
#define RENDU_APPENDER_TYPE_H
#include "common/define.h"


BEGIN_NAMESPACE_COMMON
    enum class AppenderType : uint8
    {
        APPENDER_NONE,
        APPENDER_CONSOLE,
        APPENDER_FILE,
        APPENDER_DB,

        APPENDER_INVALID = 0xFF // SKIP
    };

END_NAMESPACE_COMMON
#endif //RENDU_APPENDER_TYPE_H

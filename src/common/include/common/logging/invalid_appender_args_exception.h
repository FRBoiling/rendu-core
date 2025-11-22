//
// Created by 沸腾 on 2025/11/22.
//

#ifndef RENDU_INVALID_APPENDER_ARGS_EXCEPTION_H
#define RENDU_INVALID_APPENDER_ARGS_EXCEPTION_H

#include <stdexcept>
#include "common/define.h"

BEGIN_NAMESPACE_COMMON

namespace Logging
{
    class RC_COMMON_API InvalidAppenderArgsException : public std::length_error
    {
    public:
        explicit InvalidAppenderArgsException(std::string const& message) : std::length_error(message) { }
    };
}// namespace Logging

END_NAMESPACE_COMMON



#endif //RENDU_INVALID_APPENDER_ARGS_EXCEPTION_H
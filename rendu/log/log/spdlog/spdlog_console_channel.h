/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_SPDLOG_CONSOLE_CHANNEL_H
#define RENDU_SPDLOG_CONSOLE_CHANNEL_H

#include "spdlog_channel.h"

LOG_NAMESPACE_BEGIN

class SpdLogConsoleChannel : public SpdlogChannel {
public:
  explicit SpdLogConsoleChannel();

};

LOG_NAMESPACE_END

#endif//RENDU_SPDLOG_CONSOLE_CHANNEL_H

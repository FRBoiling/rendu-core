/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_SPDLOG_FILE_CHANNEL_H
#define RENDU_SPDLOG_FILE_CHANNEL_H

#include "spdlog_channel.h"

LOG_NAMESPACE_BEGIN

class SpdLogFileChannel : public SpdlogChannel {
public:
  explicit SpdLogFileChannel();
};

LOG_NAMESPACE_END

#endif//RENDU_SPDLOG_FILE_CHANNEL_H

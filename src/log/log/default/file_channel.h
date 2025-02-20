/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_LOG_FILE_CHANNEL_H
#define RENDU_LOG_FILE_CHANNEL_H

#include "base/log/a_logger_channel.h"
#include "log_define.h"

LOG_NAMESPACE_BEGIN

class FileChannel : public ALoggerChannel {
public:
  explicit FileChannel();

private:
};

LOG_NAMESPACE_END

#endif//RENDU_LOG_FILE_CHANNEL_H

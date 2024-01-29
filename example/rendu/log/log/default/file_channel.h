/*
* Created by boil on 2023/12/21.
*/

#ifndef RENDU_LOG_FILE_CHANNEL_H
#define RENDU_LOG_FILE_CHANNEL_H

#include "log_define.h"
#include "a_logger_channel.h"

LOG_NAMESPACE_BEGIN

class FileChannel : public ALoggerChannel {
public:
  explicit FileChannel(Logger *logger);

private:
};

LOG_NAMESPACE_END

#endif//RENDU_LOG_FILE_CHANNEL_H

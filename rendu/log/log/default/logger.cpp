/*
* Created by boil on 2024/2/4.
*/

#include "logger.h"
#include "console_channel.h"
#include "file_channel.h"

LOG_NAMESPACE_BEGIN

void Logger::InitChannel() {
  if (GetMode() == LogMode::LM_SYNC) {
    if (GetPosition() == LogPosition::LP_CONSOLE||GetPosition() == LogPosition::LP_CONSOLE_AND_FILE) {
      AddChannel(new ConsoleChannel());
    }
    if (GetPosition() == LogPosition::LP_FILE||GetPosition() == LogPosition::LP_CONSOLE_AND_FILE){
      AddChannel(new FileChannel());
    }
  }
}

LOG_NAMESPACE_END
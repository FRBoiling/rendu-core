/*
* Created by boil on 2023/11/14.
*/

#include "log_helper.h"

#include <utility>
#include "a_logger.h"

RD_NAMESPACE_BEGIN
//可重置默认值
ALogger *g_defaultLogger = nullptr;

ALogger *GetLogger() {
  if (!g_defaultLogger) {
    g_defaultLogger = new ALogger("default");
    //  logger->Add(std::make_shared<ConsoleChannel>());
    //  logger->SetWriter(std::make_shared<AsyncLogWriter>());
    //  logger->SetLevel(LogLevel::LTrace);

    g_defaultLogger->Add(std::make_shared<ALogChannel>());
    g_defaultLogger->SetWriter(std::make_shared<ALogWriter>());
    g_defaultLogger->SetLevel(LogLevel::LTrace);
  }
  return g_defaultLogger;
}

void SetLogger(ALogger* logger) {
  g_defaultLogger = logger;
}

RD_NAMESPACE_END
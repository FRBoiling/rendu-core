#ifndef RENDU_ENGINE_LOG_CPP_
#define RENDU_ENGINE_LOG_CPP_

#include "log.h"
#include <types/basic_loggers_def.h>

namespace rendu::log {
  template<traits::DefinitionLogger Logger, typename... Args>
  void Console(const Formater<Logger> format, Args &&... args) noexcept {
#if defined(RENDU_PLATFORM_WIN)
    if(_isatty(_fileno(stdout)))
#elif defined(RENDU_PLATFORM_LINUX)
    if(isatty(fileno(stdout)))
#endif
    types::BasicLogger::PrintConsole(format, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void Console(const traits::DefinitionLogger auto logger, const Formater<std::decay_t<decltype(logger)>> format,
               Args &&... args) noexcept {
#if defined(RENDU_PLATFORM_WIN)
    if(_isatty(_fileno(stdout)))
#elif defined(RENDU_PLATFORM_LINUX)
    if(isatty(fileno(stdout)))
#endif
    types::BasicLogger::PrintConsole(format, std::forward<Args>(args)...);
  }
}

#endif // RENDU_ENGINE_LOG_CPP_

/*
* Created by boil on 2024/11/15.
*/

#ifndef RENDU_CORE_CORE_WORLD_LOG_I_LOG_H_
#define RENDU_CORE_CORE_WORLD_LOG_I_LOG_H_

#include "core_define.h"

RD_NAMESPACE_BEGIN
class ILog {
  public:
    template<typename... Args>
    void Trace(Args &&... args) {
      RD_TRACE(args...);
    };

    template<typename... Args>
    void Debug(Args &&... args) {
      RD_DEBUG(args...);
    };

    template<typename... Args>
    void Info(Args &&... args) {
      RD_INFO(args...);
    };

    template<typename... Args>
    void Warn(Args &&... args) {
      RD_WARN(args...);
    };

    template<typename... Args>
    void Error(Args &&... args) {
      RD_ERROR(args...);
    };

    template<typename... Args>
    void Critical(Args &&... args) {
      RD_CRITICAL(args...);
    };

    virtual void Critical(const Exception &e) = 0;
};

RD_NAMESPACE_END

#endif//RENDU_CORE_CORE_WORLD_LOG_I_LOG_H_

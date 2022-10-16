/*
* Created by boil on 2022/10/16.
*/
#ifndef RENDU_LOG_H_
#define RENDU_LOG_H_

#include "base_logger.h"
#include "singleton.h"

namespace rendu {
  class Log : public Singleton<Log>{
  private:
    BaseLogger* logger_;
  public:

    std::shared_ptr<spdlog::logger> get_logger() { return logger_->get_logger(); }
    void set_logger(BaseLogger*& logger) { logger_ = logger; }

//    template<typename... Args>
//    void TRACE(spdlog::format_string_t<Args...> fmt, Args &&... args){
//        logger_.get_logger()->template trace(fmt, std::forward<Args>(args)...);
//    }
//    template<typename... Args>
//    void DEBUG(spdlog::format_string_t<Args...> fmt, Args &&... args){
//      logger_.get_logger()->template debug(fmt, std::forward<Args>(args)...);
//    }
//    template<typename... Args>
//    void INFO(spdlog::format_string_t<Args...> fmt, Args &&... args){
//      logger_.get_logger()->template info(fmt, std::forward<Args>(args)...);
//    }
//    template<typename... Args>
//    void WARN(spdlog::format_string_t<Args...> fmt, Args &&... args){
//      logger_.get_logger()->template warn(fmt, std::forward<Args>(args)...);
//    }
//    template<typename... Args>
//    void ERROR(spdlog::format_string_t<Args...> fmt, Args &&... args){
//      logger_.get_logger()->template error(fmt, std::forward<Args>(args)...);
//    }
//    template<typename... Args>
//    void CRITICAL(spdlog::format_string_t<Args...> fmt, Args &&... args){
//      logger_.get_logger()->template critical(fmt, std::forward<Args>(args)...);
//    }
  };

#define RD_LOG_INIT(logger)  Log::get_inst().set_logger(logger);

#define RD_LOGGER_TRACE(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::trace, __VA_ARGS__)
#define RD_LOGGER_DEBUG(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::debug, __VA_ARGS__)
#define RD_LOGGER_INFO(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::info, __VA_ARGS__)
#define RD_LOGGER_WARN(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::warn, __VA_ARGS__)
#define RD_LOGGER_ERROR(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::err, __VA_ARGS__)
#define RD_LOGGER_CRITICAL(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::critical, __VA_ARGS__)

#define RD_STOPWATCH spdlog::stopwatch
#define RD_LOGGER_INIT(logger) Log::get_inst().set_logger(logger)
#define RD_TRACE(...) RD_LOGGER_TRACE(Log::get_inst().get_logger(), __VA_ARGS__)
#define RD_DEBUG(...) RD_LOGGER_DEBUG(Log::get_inst().get_logger(), __VA_ARGS__)
#define RD_INFO(...) RD_LOGGER_INFO(Log::get_inst().get_logger(), __VA_ARGS__)
#define RD_WARN(...) RD_LOGGER_WARN(Log::get_inst().get_logger(), __VA_ARGS__)
#define RD_ERROR(...) RD_LOGGER_ERROR(Log::get_inst().get_logger(), __VA_ARGS__)
#define RD_CRITICAL(...) RD_LOGGER_CRITICAL(Log::get_inst().get_logger(), __VA_ARGS__)

}//namespace rendu

#endif //RENDU_LOG_H_

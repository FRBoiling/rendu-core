/*
* Created by boil on 2024/6/21.
*/

#ifndef RENDU_EXAMPLE_BASE_LOG_SPDLOG_EXAMPLE_H_
#define RENDU_EXAMPLE_BASE_LOG_SPDLOG_EXAMPLE_H_


#include <spdlog/spdlog.h>

void spdlog_example(){
  spdlog::set_level(spdlog::level::trace); // or spdlog::set_level(spdlog::level::trace);
  SPDLOG_TRACE("SPDLOG_TRACE");
  SPDLOG_DEBUG("SPDLOG_DEBUG");
  SPDLOG_INFO("SPDLOG_INFO");
  SPDLOG_ERROR("SPDLOG_ERROR");
}


#endif//RENDU_EXAMPLE_BASE_LOG_SPDLOG_EXAMPLE_H_

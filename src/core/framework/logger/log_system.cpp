/*
* Created by boil on 2022/10/16.
*/
#include "log_system.h"
#include "logger.h"

using namespace rendu;
void LogSystem::Initialize(const std::string &flag, RunModeType mode, const std::string &path) {
  logger_.init(flag, mode, path);
  Log::get_inst().set_logger(logger_);
}

void LogSystem::Register() {
  Initialize(sOptions.m_program_name, sOptions.m_run_mode, "");
}

void LogSystem::Destroy() {

}



/*
* Created by boil on 2022/10/16.
*/
#include "log_system.h"
#include "logger.h"

using namespace rendu;
void LogSystem::Initialize(const std::string &flag, RunModeType mode,const std::string &path) {
  logger_ = std::make_shared<Logger>(flag,mode,path);
//  RD_LOG_INIT(logger_);
}

void LogSystem::Register() {
  Initialize(sOptions.m_program_name, sOptions.m_run_mode, "");
}

void LogSystem::Destroy() {

}

string LogSystem::Name() {
  return Type().name();
}

const std::type_info &LogSystem::Type() {
  return typeid(this);
}



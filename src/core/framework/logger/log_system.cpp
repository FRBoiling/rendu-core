/*
* Created by boil on 2022/10/16.
*/
#include "log_system.h"
#include "rd_logger.h"

using namespace rendu;
void LogSystem::Initialize(const std::string &flag, RunModeType mode, const std::string &path) {
  RDLogger logger;
  logger.init(flag, mode, path);
  RD_LOGGER_INIT(logger);
}

void LogSystem::Register() {
  Initialize(sOptions.m_program_option.name(), sOptions.m_program_option.run_mode(), "");
}

void LogSystem::Destroy() {

}

void LogSystem::Update(uint64 dt) {

}

void LogSystem::Exit() {

}

const std::type_info &LogSystem::GetType() {
  return typeid(this);
}


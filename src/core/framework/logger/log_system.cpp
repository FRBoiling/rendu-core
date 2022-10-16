/*
* Created by boil on 2022/10/16.
*/
#include "log_system.h"
#include "rd_logger.h"

namespace rendu{
  void LogSystem::Initialize(const std::string &flag, RunModeType mode, const std::string &path) {
    RDLogger logger;
    logger.init(flag,mode,path);
    Log::get_inst().set_logger(logger);
  }

  void LogSystem::Register() {

  }

  void LogSystem::Destroy() {

  }

}
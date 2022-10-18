/*
* Created by boil on 2022/9/9.
*/
#ifndef RENDU_LOG_SYSTEM_H_
#define RENDU_LOG_SYSTEM_H_

#include <log.h>
#include "base_logger.h"
#include "singleton.h"
#include "define.h"
#include "base_system.h"
#include "run_mode.h"
#include "logger.h"

namespace rendu {

  class LogSystem : public Singleton<LogSystem>,public BaseSystem {
  public:
    void Initialize(const std::string &flag, RunModeType mode, const std::string &path);
    void Register() override;
    void Destroy() override;

  private:
    Logger logger_;
  };

#define sLogger LogSystem::get_inst()

}//namespace rendu

#endif // RENDU_LOG_SYSTEM_H_

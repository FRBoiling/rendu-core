/*
* Created by boil on 2022/9/9.
*/
#ifndef RENDU_LOG_SYSTEM_H_
#define RENDU_LOG_SYSTEM_H_

#include <log.h>
#include "a_logger.h"
#include "singleton.h"
#include "define.h"
#include "proto/core/model/program_args.pb.h"

namespace rendu {
  using namespace model;

  class LogSystem : public Singleton<LogSystem>,public ISystem {
  public:
    void Initialize(const std::string &flag, RunModeType mode, const std::string &path);
    void Register() override;

    void Destroy() override;

    void Update(uint64 dt) override;

    void Exit() override;

    const std::type_info &GetType() override;
  };

#define sLogger LogSystem::get_inst()

}//namespace rendu

#endif // RENDU_LOG_SYSTEM_H_

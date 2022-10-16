/*
* Created by boil on 2022/8/27.
*/

#ifndef RENDU_PROGRAM_H_
#define RENDU_PROGRAM_H_

#include <map>
#include "define.h"
#include "base_component.h"
#include "base_system.h"
#include "singleton.h"
#include "interface_update.h"
#include "logger.h"

namespace rendu {
  RD_FRAMEWORK_API void Run();

  RD_FRAMEWORK_API void Initialize(int argc, char **argv);

  RD_FRAMEWORK_API BaseSystem &AddSingleton(BaseSystem &system);

  class Program : public BaseSystem, public IUpdate {

  public:
    BaseSystem &AddSingleton(BaseSystem &system);

    bool IsRunning();

    void Update() override;

    void Register() override;

    void Destroy() override;

  private:
    std::map<size_t, BaseSystem *> systems_;
  };

}//namespace rendu::program

#endif //RENDU_PROGRAM_H_
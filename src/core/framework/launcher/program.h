/*
* Created by boil on 2022/8/27.
*/

#ifndef RENDU_PROGRAM_H_
#define RENDU_PROGRAM_H_

#include <map>
#include "component.h"
#include "system.h"
#include "options_system.h"
#include "singleton.h"

namespace rendu {


  class Program : public Singleton<Program>, public ISystem {
  public:

  public:
    void Initialize(int argc, char **argv);

    ISystem &AddSystem(ISystem &system);

    void Run();

    bool IsRunning();

    void Register() override;

    void Update(uint64 dt) override;

    void Exit() override;

    void Destroy() override;

  private:
    SystemState state_;
    std::map<size_t, ISystem *> systems_;
  };

#define sProgram Program::get_inst()
}//namespace rendu

#endif //RENDU_PROGRAM_H_
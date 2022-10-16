/*
* Created by boil on 2022/9/4.
*/

#ifndef RENDU_OPTIONS_SYSTEM_H_
#define RENDU_OPTIONS_SYSTEM_H_

#include "program.h"
#include "singleton.h"
#include "program_args.pb.h"
#include <string>

namespace rendu {
  using namespace model;

  class OptionsSystem : public Singleton<OptionsSystem> ,public ISystem{

  public:
    int Parse(int argc, char **argv);
  private:
    void Register() override;

    void Destroy() override;

  public:
    model::ProgramOption m_program_option;
    void Show();
  };

#define sOptions OptionsSystem::get_inst()
}//namespace rendu




#endif //RENDU_OPTIONS_SYSTEM_H_

/*
* Created by boil on 2022/8/27.
*/

#include "program.h"
#include "config_system.h"
#include "event_system.h"

using namespace rendu;

/// Launch the rendu core program
extern int main(int argc, char **argv) {
  rendu::Initialize(argc, argv);
  rendu::AddSingleton(ConfigSystem::get_inst());
//  rendu::AddSingleton(EventSystem::get_inst());
// rendu::AddSystem(EventSystem::get_inst());
  rendu::Run();
  return 0;
}
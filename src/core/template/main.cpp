/*
* Created by boil on 2022/8/27.
*/

#include "program.h"

#include "log_system.h"
#include "config_system.h"
#include "event_system.h"

using namespace rendu;
/// Launch the rendu core program
extern int main(int argc, char **argv) {
  sProgram.Initialize(argc,argv);
  sProgram.AddSystem(ConfigSystem::get_inst());
//  sProgram.AddSystem(EventSystem::get_inst());
  sProgram.Run();
  return 0;
}
/*
* Created by boil on 2022/8/27.
*/

#include "options_parser.h"
#include "program.h"

#include "log_system.h"
#include "config_system.h"
#include "event_system.h"

using namespace rendu;

/// Launch the rendu core program
extern int main(int argc, char **argv) {
  rendu::Parse(argc, argv, sOptions);
  rendu::AddSingleton(LogSystem::get_inst());
//  rendu::AddSingleton(ConfigSystem::get_inst());
//  rendu::AddSingleton(EventSystem::get_inst());
// rendu::AddSystem(EventSystem::get_inst());
  rendu::Run();
  return 0;
}
/*
* Created by boil on 2022/8/27.
*/
#include "program.h"
#include "log_system.h"
#include "enum.h"
#include "options_system.h"

using namespace rendu;

void Program::Initialize(int argc, char **argv) {
  sOptions.Parse(argc,argv);
  sLogger.Initialize(sOptions.m_program_option.name(), sOptions.m_program_option.run_mode(), "");
  sOptions.Show();
  RD_INFO("initialize...");

  RD_INFO("initialize success!");
  RD_INFO("ProgramState is {}", rendu::enum_name(_state));
  _state = ProgramState::INITIALIZED;
}

bool Program::IsStopped() {
  return _state == ProgramState::STOPPED;
}

void Program::Exit() {
  RD_INFO("exit ....!");
  _state = ProgramState::STOPPING;
}

void Program::Run() {
  Start();
  while (!IsStopped()) {
    Update();
  }
  Stop();
  RD_INFO("exit success!");
}

void Program::Start() {
  if (_state == ProgramState::INITIALIZED) {
    RD_INFO("start...");
    //TODO:BOIL
    RD_INFO("start success!");
    _state = ProgramState::RUNNING;
  }
}

void Program::Stop() {
  if (_state == ProgramState::STOPPED) {
    _state = ProgramState::STOPPING;
    RD_INFO("stop...\n");
    //TODO:BOIL
    RD_INFO("stop success!\n");
  }
}

void Program::Update() {

}

ISystem& Program::AddSystem(ISystem &system) {
  auto hash_code = system.GetType().hash_code();
  if (_updates.contains(hash_code)) {
        RD_DEBUG("AddSystem fail ! The {} already exist.", system.ToString());
  }
  _updates[hash_code] = &system;
  RD_DEBUG("AddSystem {} success !", system.ToString());
  return system;
}






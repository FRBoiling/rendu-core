/*
* Created by boil on 2022/8/27.
*/
#include "options_parser.h"
#include "program.h"
#include "options.h"
#include "log.h"

namespace rendu {

  Program program;

  void Run() {
    program.Register();

//    while (program.IsRunning()) {
//      program.Update();
//    }
    program.Destroy();
    RD_INFO("exit success!");
  }

  void Initialize(int argc, char **argv) {
    Parse(argc, argv, sOptions);
    BaseLogger *logger = new Logger(sOptions.m_program_name, sOptions.m_run_mode, "");
    RD_LOGGER_INIT(logger);
    Show();
  }

  BaseSystem &AddSingleton(BaseSystem &system) {
    return program.AddSingleton(system);
  }

  bool Program::IsRunning() {
    return GetState() == SystemState::Running;
  }

  void Program::Destroy() {
    if (CheckState(SystemState::Exit)) {
      SetState(SystemState::Destroy);
      RD_INFO("stop...\n");
      //TODO:BOIL
      RD_INFO("stop success!\n");
    }
  }

  BaseSystem &Program::AddSingleton(BaseSystem &system) {
    auto hash_code = system.Type().hash_code();
    if (systems_.contains(hash_code)) {
      RD_INFO("AddSystem fail ! The {} already exist.", system.Name());
    }

    systems_[hash_code] = &system;
    RD_INFO("AddSystem {} success !", system.Name());
    return system;
  }

  void Program::Register() {
    for (auto it: systems_) {
      it.second->Register();
      RD_INFO("{}--{} Register", it.first, it.second->Name());
    }
    SetState(SystemState::Running);
    RD_INFO("Register complete !  current systems count is {} .", systems_.size());
  }

  void Program::Update() {
    for (auto it: systems_) {
      RD_INFO("{}--{} Update", it.first, it.second->Name());
    }
  }

}// namespace rendu
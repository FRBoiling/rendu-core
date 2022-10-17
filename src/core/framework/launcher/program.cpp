/*
* Created by boil on 2022/8/27.
*/
#include "program.h"
#include "log_system.h"
#include "options_system.h"

using namespace rendu;

void Program::Initialize(int argc, char **argv) {
  sOptions.Parse(argc, argv);
}

bool Program::IsRunning() {
  return state_ == SystemState::Running;
}

void Program::Run() {
  Register();
  while (IsRunning()) {
    Update(1);
  }
  Destroy();
  RD_INFO("exit success!");
}

void Program::Destroy() {
  if (state_ == SystemState::Exit) {
    state_ = SystemState::Destroy;
    RD_INFO("stop...\n");
    //TODO:BOIL
    RD_INFO("stop success!\n");
  }
}

ISystem &Program::AddSystem(ISystem &system) {
  auto hash_code = system.GetType().hash_code();
  if (systems_.contains(hash_code)) {
    RD_DEBUG("AddSystem fail ! The {} already exist.", system.ToString());
  }

  systems_[hash_code] = &system;
  RD_DEBUG("AddSystem {} success !", system.ToString());
  return system;
}

void Program::Register() {
  for(auto it : systems_){
    it.second->Register();
    RD_DEBUG("{}--{} Register",it.first ,it.second->ToString());
  }
  sOptions.Show();
  state_ = SystemState::Running;
  RD_INFO("Register success !  current systems count is {} .", systems_.size());
}

void Program::Update(uint64 dt) {
  for(auto it : systems_){
    it.second->Update(dt);
    RD_DEBUG("{}--{} Update",it.first ,it.second->ToString());
  }
}

void Program::Exit() {
  for(auto it : systems_){
    it.second->Exit();
    RD_DEBUG("{}--{} Exit",it.first ,it.second->ToString());
  }
}






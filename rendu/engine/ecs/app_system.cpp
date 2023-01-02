/*
* Created by boil on 2023/1/2.
*/

#include "app_system.h"
#include "log/logger.h"

bool AppSystem::Initialize() {
  m_system_state = SystemState::INITIALIZE;
  RD_LOG_INFO("AppSystem state {}", "");
  return false;
}

bool AppSystem::Destroy() {
  m_system_state = SystemState::DESTROY;
  return false;
}

void AppSystem::Update() {
}

void AppSystem::Run() {
  m_system_state = SystemState::UPDATE;
  Initialize();
  while (IsRunning()){
    Update();
  }
  Destroy();
}

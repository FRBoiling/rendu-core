/*
* Created by boil on 2023/1/2.
*/

#include "app_system.h"
#include "log/logger.h"
#include "enum/enum_helper.h"

bool AppSystem::Initialize() {
  m_system_state = SystemState::INITIALIZE;
  RD_LOG_INFO("AppSystem state {}", EnumToString(m_system_state));
  return false;
}

bool AppSystem::Destroy() {
  m_system_state = SystemState::DESTROY;
  RD_LOG_INFO("AppSystem state {}", EnumToString(m_system_state));
  return false;
}

void AppSystem::Update() {
  if (m_system_state == SystemState::STOP) {
    RD_LOG_INFO("AppSystem state {}", EnumToString(m_system_state));
    m_system_state = SystemState::DESTROY;
  }
  if (m_system_state == SystemState::INITIALIZE) {
    m_system_state = SystemState::UPDATE;
    RD_LOG_INFO("AppSystem state {}", EnumToString(m_system_state));
  }
}


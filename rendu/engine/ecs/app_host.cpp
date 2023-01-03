/*
* Created by boil on 2023/1/2.
*/

#include "app_host.h"

int AppHost::Run(int argc, char **argv) {
  m_app_system.Initialize();
  while (m_app_system.IsRunning()) {
    m_app_system.Update();
  }
  m_app_system.Destroy();
  return 0;
}

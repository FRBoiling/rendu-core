/*
* Created by boil on 2023/1/2.
*/

#ifndef RENDU_CORE_APP_SYSTEM_H_
#define RENDU_CORE_APP_SYSTEM_H_
#include "a_system.h"
#include "app_entity.h"

class AppSystem : public ASystem{
public:
  bool Initialize() override;

  bool Destroy() override;

  void Update() override;
public:
  void Run();
public:
  AppEntity m_app_entity;
};


#endif //RENDU_CORE_APP_SYSTEM_H_

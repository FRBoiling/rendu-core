/*
* Created by boil on 2023/1/2.
*/

#ifndef RENDU_CORE_APP_HOST_H_
#define RENDU_CORE_APP_HOST_H_


#include "app_entity.h"
#include "app_system.h"

class AppHost {
public:
  AppEntity m_app_entity;
  AppSystem m_app_system;
public:
  int Run(int argc, char** argv);
};


#endif //RENDU_CORE_APP_HOST_H_

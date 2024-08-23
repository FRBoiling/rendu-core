/*
* Created by boil on 2024/11/14.
*/

#ifndef RENDU_LOADER_RUNTIME_RUNTIME_HOST_H_
#define RENDU_LOADER_RUNTIME_RUNTIME_HOST_H_

#include "loader_define.h"

RD_NAMESPACE_BEGIN

class Host {
public:
  Bool isRunning = true;

public:
  void Start();
  void Update();
  void LateUpdate();
};

RD_NAMESPACE_END

#endif//RENDU_LOADER_RUNTIME_RUNTIME_HOST_H_

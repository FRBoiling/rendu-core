/*
* Created by boil on 2022/12/31.
*/

#ifndef RENDU_CORE_A_SYSTEM_H_
#define RENDU_CORE_A_SYSTEM_H_

#include "a_entity.h"

enum class SystemState :int{
  IDLE = 0,
  INITIALIZE = 1,
  UPDATE = 2,
  STOP = 3,
  DESTROY = 4,
};

class ASystem {
public:
  virtual bool Initialize() = 0;

  virtual bool Destroy() = 0;

  virtual void Update() = 0;

public:
  SystemState m_system_state;

  bool IsRunning() const { return m_system_state < SystemState::DESTROY; }
};

#endif //RENDU_CORE_A_SYSTEM_H_

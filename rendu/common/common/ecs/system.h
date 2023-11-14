/*
* Created by boil on 2023/3/19.
*/

#ifndef RENDU_COMMON_SYSTEM_H
#define RENDU_COMMON_SYSTEM_H

#include "common_fwd.h"
#include "entity_pool.h"

COMMON_NAMESPACE_BEGIN

enum class SystemType {
  Default,
  Init,
  LateInit,
  Update,
  LateUpdate,
};

class BaseSystem {
public:
  BaseSystem() : m_Type(SystemType::Default){};
  ~BaseSystem() = default;

public:
  SystemType GetSystemType() { return m_Type; }

  virtual void Run(Entity &entity) {
    LOG_INFO << ToString() << " Poll";
  };

  virtual STRING ToString() { return typeid(this).name(); }

protected:
  SystemType m_Type;
};

class BaseInitSystem : public BaseSystem {
public:
  BaseInitSystem() {
    m_Type = SystemType::Init;
  };

public:
  STRING ToString() override { return typeid(this).name(); }
};

class BaseUpdateSystem : public BaseSystem {
public:
  BaseUpdateSystem() {
    m_Type = SystemType::Update;
  };

public:
  STRING ToString() override { return typeid(this).name(); }
};

COMMON_NAMESPACE_END

#endif//RENDU_COMMON_COMPONENT_SYSTEM_H

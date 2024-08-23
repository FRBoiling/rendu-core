/*
* Created by boil on 2024/11/14.
*/

#ifndef RENDU_CORE_CORE_ENTITY_ENTITY_H_
#define RENDU_CORE_CORE_ENTITY_ENTITY_H_

#include "core_define.h"
#include "object/dispose_object.h"

RD_NAMESPACE_BEGIN
enum EntityStatus {
  None = 0,
  IsFromPool = 1,
  IsRegister = 1 << 1,
  IsComponent = 1 << 2,
  IsNew = 1 << 3,
  IsSerializeWithParent = 1 << 4,
};

class Entity : public DisposeObject {
  public:
    Entity(Long instanceId, Long id) : m_instanceId(instanceId), m_id(id) {
    }
    void Dispose() override {
    }
    void BeginInit() override {
    }
    void EndInit() override {
    }

  protected:
    Long m_id;
    Long m_instanceId;
};

RD_NAMESPACE_END

#endif//RENDU_CORE_CORE_ENTITY_ENTITY_H_

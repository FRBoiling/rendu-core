/*
* Created by boil on 2023/9/26.
*/

#ifndef RENDU_ACTOR_ID_H
#define RENDU_ACTOR_ID_H

#include "actor_address.h"
#include "core_define.h"

CORE_NAMESPACE_BEGIN

class ActorId {

public:
  ActorAddress GetActorAddress() const{
    return m_address;
  }

  uLong GetInstanceId() const{
    return m_instance_id;
  }

private:
  ActorAddress m_address;
  uLong m_instance_id;
};

CORE_NAMESPACE_END

#endif//RENDU_ACTOR_ID_H

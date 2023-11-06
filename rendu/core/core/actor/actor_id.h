/*
* Created by boil on 2023/9/26.
*/

#ifndef RENDU_ACTOR_ID_H
#define RENDU_ACTOR_ID_H

#include "common/define.h"
#include "address.h"

RD_NAMESPACE_BEGIN

    class ActorId {

    public:
      Address Address;
      long InstanceId;

    };

RD_NAMESPACE_END

#endif //RENDU_ACTOR_ID_H

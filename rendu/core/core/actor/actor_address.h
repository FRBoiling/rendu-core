/*
* Created by boil on 2023/9/26.
*/

#ifndef RENDU_ADDRESS_H
#define RENDU_ADDRESS_H

#include "core_define.h"

RD_NAMESPACE_BEGIN

    class ActorAddress {
    public :
      int Process;
      int Fiber;

    };

RD_NAMESPACE_END

#endif //RENDU_ADDRESS_H

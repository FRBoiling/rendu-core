/*
* Created by boil on 2023/10/17.
*/

#ifndef RENDU_NET_SERVICES_H
#define RENDU_NET_SERVICES_H


#include "common/define.h"
#include "common/utils/singleton.h"


RD_NAMESPACE_BEGIN

    class NetServices : public Singleton<NetServices> {
    public:
      int64 CreateAcceptChannelId();
    };

RD_NAMESPACE_END

#endif //RENDU_NET_SERVICES_H

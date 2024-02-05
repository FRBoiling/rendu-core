/*
* Created by boil on 2023/10/20.
*/

#include "outer_net_system.h"
#include "network/t_service.h"

CORE_NAMESPACE_BEGIN
    void OuterNetSystem::Awake() {
      auto ipEndPoint = IPEndPoint::FromString("127.0.0.1:11111");
      m_service = new TService(ipEndPoint.value(), ServiceType::Inner);
      SystemAwake::Awake();
    }


CORE_NAMESPACE_END



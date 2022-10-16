/*
* Created by boil on 2022/8/28.
*/

#ifndef RENDU_CONFIG_SYSTEM_H_
#define RENDU_CONFIG_SYSTEM_H_

#include "base_system.h"
#include "singleton.h"

namespace rendu {
  class ConfigSystem : public Singleton<ConfigSystem>, public BaseSystem {
  public:
    void Register() override;

    void Destroy() override;

  };
}//namespace rendu

#endif //RENDU_CONFIG_SYSTEM_H_

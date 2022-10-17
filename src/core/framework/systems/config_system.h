/*
* Created by boil on 2022/8/28.
*/

#ifndef RENDU_CONFIG_SYSTEM_H_
#define RENDU_CONFIG_SYSTEM_H_

#include "system.h"
#include "singleton.h"

namespace rendu {
  class ConfigSystem : public Singleton<ConfigSystem>, public ISystem {
  public:
    void Register() override;

    void Destroy() override;

    const std::type_info &GetType() override;

    void Update(uint64 dt) override;

    void Exit() override;

  };
}//namespace rendu

#endif //RENDU_CONFIG_SYSTEM_H_

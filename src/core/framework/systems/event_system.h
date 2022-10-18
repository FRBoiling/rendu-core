/*
* Created by boil on 2022/8/28.
*/

#ifndef RENDU_EVENT_SYSTEM_H_
#define RENDU_EVENT_SYSTEM_H_

#include "base_system.h"
#include "singleton.h"

namespace rendu{

class EventSystem : public Singleton<EventSystem>, public BaseSystem {

public:
  void Register() override;

  void Destroy() override;
};

}//namespace rendu

#endif //RENDU_EVENT_SYSTEM_H_

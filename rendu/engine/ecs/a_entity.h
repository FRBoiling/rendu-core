/*
* Created by boil on 2022/12/31.
*/

#ifndef RENDU_CORE_A_ENTITY_H_
#define RENDU_CORE_A_ENTITY_H_

#include "a_component.h"
#include <map>

class AEntity : public AComponent {
public:
  std::map<int, AComponent> m_components;
};

#endif //RENDU_CORE_A_ENTITY_H_

/*
* Created by boil on 2022/12/31.
*/

#ifndef RENDU_CORE_I_SYSTEM_H_
#define RENDU_CORE_I_SYSTEM_H_
#include "a_entity.h"

class ISystem {
public:
  virtual bool Initialize()=0;
  virtual bool Destroy() =0;
  virtual void Update() =0;
};


#endif //RENDU_CORE_I_SYSTEM_H_

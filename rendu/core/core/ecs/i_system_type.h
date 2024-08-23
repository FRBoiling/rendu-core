/*
* Created by boil on 2024/11/15.
*/

#ifndef RENDU_CORE_CORE_ECS_I_SYSTEM_TYPE_H_
#define RENDU_CORE_CORE_ECS_I_SYSTEM_TYPE_H_

#include "core_define.h"

RD_NAMESPACE_BEGIN
class ISystemType {
  public:
    virtual ~ISystemType() = default;
    // 声明两个纯虚函数
    virtual String GetType() = 0;
    virtual String GetSystemType() = 0;
};

RD_NAMESPACE_END

#endif//RENDU_CORE_CORE_ECS_I_SYSTEM_TYPE_H_

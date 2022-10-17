/*
* Created by boil on 2022/8/28.
*/

#ifndef RENDU_SYSTEM_H_
#define RENDU_SYSTEM_H_

#include <typeinfo>
#include <string>
#include <define.h>

namespace rendu {


  enum class SystemState {
    Idle = 0,     //闲置
    Register,     //注册
    Running,      //正在运行
    Exit,         //退出
    Destroy       //销毁
  };

  class ISystem {
  public:
    virtual void Register() = 0;

    virtual void Update(uint64 dt) = 0;

    virtual void Exit() = 0;

    virtual void Destroy() = 0;

    virtual std::string ToString() {
      return GetType().name();
    }

    virtual const std::type_info &GetType() {
      return typeid(this);
    };

  };
} //namespace rendu
#endif //RENDU_SYSTEM_H_

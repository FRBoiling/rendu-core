/*
* Created by boil on 2022/8/28.
*/

#ifndef RENDU_BASE_SYSTEM_H_
#define RENDU_BASE_SYSTEM_H_

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

  class BaseSystem {
  public:
    BaseSystem() : state_(SystemState::Idle) {}

  private:
    SystemState state_;
  public:
    SystemState GetState() const {
      return state_;
    }

    void SetState(SystemState state) {
      state_ = state;
    }

    bool CheckState(SystemState state) const {
      return state_ == state;
    }

  public:
    virtual void Register() = 0;

    virtual void Destroy() = 0;

    virtual std::string Name() {
      return Type().name();
    }

    virtual const std::type_info &Type() {
      return typeid(this);
    }

  };
} //namespace rendu
#endif //RENDU_BASE_SYSTEM_H_

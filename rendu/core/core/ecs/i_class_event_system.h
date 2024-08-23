/*
* Created by boil on 2024/11/15.
*/

#ifndef RENDU_CORE_CORE_ECS_I_CLASS_EVENT_SYSTEM_H_
#define RENDU_CORE_CORE_ECS_I_CLASS_EVENT_SYSTEM_H_

#include "entity.h"
#include "i_system_type.h"

RD_NAMESPACE_BEGIN
template<typename T>
class IClassEvent {
};

// 定义一个事件系统的接口
class IClassEventSystem {
  public:
    virtual ~IClassEventSystem() = default;
    // 事件系统的其他方法和属性
};

// 使用模板和继承来实现类似的泛型约束
template<typename T>
class AClassEventSystem : public ISystemType, IClassEventSystem {
  public:
    // 确保T是一个结构体
    static_assert(std::is_class<T>::value, "T must be a class or struct");

    virtual void Run(Entity e, T t) = 0;
};

// 定义一个事件系统的抽象类
template<typename E, typename T>
class ClassEventSystem : public AClassEventSystem<T> {
  public:
    // 确保E是Entity的子类，T是结构体
    static_assert(std::is_base_of<Entity, E>::value, "E must be a subclass of Entity");
    static_assert(std::is_class<T>::value, "T must be a class or struct");

    void Run(Entity e, T t) override {
      this->Handle(static_cast<E>(e), t);
    }

    virtual String SystemType() {
      return typeid(AClassEventSystem<T>).name();
    }

    virtual String Type() {
      return typeid(E).name();
    }

  protected:
    virtual void Handle(Entity e, T t) override = 0;
};

RD_NAMESPACE_END
#endif//RENDU_CORE_CORE_ECS_I_CLASS_EVENT_SYSTEM_H_

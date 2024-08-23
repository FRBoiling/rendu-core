/*
* Created by boil on 2024/11/15.
*/

#ifndef RENDU_CORE_CORE_ECS_I_UPDATE_SYSTEM_H_
#define RENDU_CORE_CORE_ECS_I_UPDATE_SYSTEM_H_

#include "i_class_event_system.h"

RD_NAMESPACE_BEGIN
// 定义一个事件结构体
struct UpdateEvent {
};

// 定义一个事件系统的接口
class IUpdate : IClassEvent<UpdateEvent> {
  public:
    virtual ~IUpdate() = default;
    // 事件系统的其他方法和属性
};

// 使用模板和继承来实现类似的泛型约束
template<typename T>
class UpdateSystem : public ClassEventSystem<T, UpdateEvent>, IUpdate {
  public:
    // 确保T是Entity的子类
    static_assert(std::is_base_of<Entity, T>::value, "T must be a subclass of Entity");

    virtual void Update(T self) = 0;
};

RD_NAMESPACE_END

#endif//RENDU_CORE_CORE_ECS_I_UPDATE_SYSTEM_H_

#pragma once

#include "common/define.h"

#include <string>

BEGIN_NAMESPACE_COMMON
    namespace Ecs
    {
        // 组件基类，所有组件都需要继承自这个类
        class IComponent
        {
        public:
            virtual ~IComponent() = default;

            // 获取组件类型ID
            virtual std::string get_type_id() const = 0;

            // 序列化组件数据
            virtual std::string serialize() const = 0;

            // 反序列化组件数据
            virtual void deserialize(const std::string& data) = 0;
        };
    } // namespace Ecs

END_NAMESPACE_COMMON

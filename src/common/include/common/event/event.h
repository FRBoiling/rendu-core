#pragma once

#include "common/define.h"
#include <string>
#include <memory>
#include <typeinfo>

BEGIN_NAMESPACE_COMMON
namespace event {

/**
 * @brief 事件基类
 *
 * 所有事件的基类，提供事件类型标识
 */
class Event {
public:
    Event() = default;
    virtual ~Event() = default;

    /**
     * @brief 获取事件类型
     * @return 事件类型字符串
     */
    virtual std::string type() const = 0;

    /**
     * @brief 获取事件的类型信息
     * @return type_info 引用
     */
    virtual const std::type_info& type_info() const = 0;

    /**
     * @brief 克隆事件（用于异步发布）
     * @return 事件的副本
     */
    virtual std::shared_ptr<Event> clone() const = 0;
};

/**
 * @brief 模板化事件基类
 *
 * 为具体事件类型提供便捷的类型标识
 * @tparam T 事件类型
 */
template<typename T>
class TypedEvent : public Event {
public:
    /**
     * @brief 获取静态类型字符串
     * @return 事件类型字符串
     */
    static std::string static_type() {
        return typeid(T).name();
    }

    /**
     * @brief 获取事件类型
     * @return 事件类型字符串
     */
    std::string type() const override {
        return static_type();
    }

    /**
     * @brief 获取事件的类型信息
     * @return type_info 引用
     */
    const std::type_info& type_info() const override {
        return typeid(T);
    }

    /**
     * @brief 克隆事件
     * @return 事件的副本
     */
    std::shared_ptr<Event> clone() const override {
        return std::make_shared<T>(static_cast<const T&>(*this));
    }
};

} // namespace event
END_NAMESPACE_COMMON

//
// Created by FRee2 on 2025/10/18.
//

#ifndef RENDUCORE_ISYSTEM_H
#define RENDUCORE_ISYSTEM_H

#include <entt/entt.hpp>
#include <string>

// 系统基类 - 所有系统都继承自这个类
class ISystem {
public:
  virtual ~ISystem() = default;
  virtual void configure(entt::registry& registry) = 0;
  virtual void update(entt::registry& registry, float delta_time) = 0;
  virtual void initialize(entt::registry& registry) = 0;
  virtual void shutdown(entt::registry& registry) = 0;
  virtual std::string get_name() const = 0;
};



#endif // RENDUCORE_ISYSTEM_H

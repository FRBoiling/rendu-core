/*
* Created by boil on 2023/3/26.
*/
#include <string>
#include <entity/registry.hpp>

//为了使用entt实现ECS（Entity-Component-System）架构，您可以按照以下步骤操作：

//1.使用entt::component定义组件：
struct Position {
  float x;
  float y;
};

struct Velocity {
  float x;
  float y;
};

struct Sprite {
  std::string texturePath;
};

int main() {
  //2.使用entt::entity定义实体：
  entt::registry registry;
  auto entity = registry.create();
  //3.使用entt::registry::emplace将组件分配给实体：
  registry.emplace<Position>(entity, 0.0f, 0.0f);
  registry.emplace<Velocity>(entity, 1.0f, 1.0f);
  registry.emplace<Sprite>(entity, "path/to/texture.png");
  //4.使用entt::registry::get访问实体的组件：
  auto &position = registry.get<Position>(entity);
  auto &velocity = registry.get<Velocity>(entity);
  auto &sprite = registry.get<Sprite>(entity);

  //5.使用entt::registry::view迭代具有特定组件的实体：
  for (auto entity : registry.view<Position, Velocity>()) {
    auto &position = registry.get<Position>(entity);
    auto &velocity = registry.get<Velocity>(entity);
    // ...
  }

}


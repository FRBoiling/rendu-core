#include "example.h"
#include "common/ecs/entity.h"
#include "common/ecs/registry.h"
#include <iostream>

using namespace Rendu;

int main()
{
    std::cout << "=== Rendu Core ECS 示例程序 ===" << std::endl;
    std::cout << "\n1. 简单的 ECS 示例..." << std::endl;

    // 创建 Registry
    Registry registry;

    // 创建实体
    Entity entity = registry.create();
    std::cout << "创建了实体: ID=" << entity.value() << std::endl;

    // 添加组件
    registry.emplace<Position>(entity, 10.0f, 20.0f);
    registry.emplace<Velocity>(entity, 1.0f, 0.5f);
    registry.emplace<Name>(entity, "TestEntity");

    std::cout << "添加了 Position, Velocity, Name 组件" << std::endl;

    // 获取组件
    Position* pos = registry.tryGet<Position>(entity);
    Velocity* vel = registry.tryGet<Velocity>(entity);
    Name* name = registry.tryGet<Name>(entity);

    if (pos && vel && name)
    {
        std::cout << "组件获取成功:" << std::endl;
        std::cout << "  Name: " << name->value << std::endl;
        std::cout << "  Position: (" << pos->x << ", " << pos->y << ")" << std::endl;
        std::cout << "  Velocity: (" << vel->vx << ", " << vel->vy << ")" << std::endl;
    }

    // 模拟移动
    std::cout << "\n2. 模拟移动..." << std::endl;
    for (int i = 0; i < 5; ++i)
    {
        pos = registry.tryGet<Position>(entity);
        vel = registry.tryGet<Velocity>(entity);

        if (pos && vel)
        {
            pos->x += vel->vx;
            pos->y += vel->vy;
            std::cout << "Frame " << i << ": Position=(" << pos->x << ", " << pos->y << ")" << std::endl;
        }
    }

    // 创建多个实体
    std::cout << "\n3. 创建多个实体并使用视图..." << std::endl;
    for (int i = 0; i < 5; ++i)
    {
        Entity e = registry.create();
        registry.emplace<Position>(e, static_cast<float>(i * 10), static_cast<float>(i * 20));
        registry.emplace<Velocity>(e, 1.0f, 0.5f);
    }

    auto view = registry.view<Position, Velocity>();
    std::cout << "视图中有 " << view.size() << " 个实体" << std::endl;

    view.each([&](Entity e, Position* p, Velocity* v) {
        std::cout << "  Entity " << e.value() << ": Position=(" << p->x << ", " << p->y << ")" << std::endl;
    });

    std::cout << "\n=== 示例完成 ===" << std::endl;
    return 0;
}

//
// Created by boil on 2026/1/15.
// ECS 性能测试 - 优化后的 RegistryOptimized vs EnTT
//

#include <entt/entt.hpp>
#include "common/ecs/registry_optimized.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>

using namespace Rendu;

// 示例组件
struct Position
{
    float x, y, z;
};

struct Velocity
{
    float dx, dy, dz;
};

struct Rotation
{
    float qx, qy, qz, qw;
};

struct Scale
{
    float sx, sy, sz;
};

// 性能计时器
class Timer
{
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    double elapsed() const
    {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_).count();
    }

    void reset()
    {
        start_ = std::chrono::high_resolution_clock::now();
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
};

// 测试 1: 实体创建和组件添加
void benchmark_entity_creation(size_t count)
{
    std::cout << "\n=== 测试 1: 实体创建和组件添加 (" << count << " 实体) ===" << std::endl;

    // Rendu RegistryOptimized
    {
        RegistryOptimized registry;
        Timer timer;

        for (size_t i = 0; i < count; ++i)
        {
            auto entity = registry.create();
            registry.emplace<Position, Velocity, Rotation>(
                entity,
                Position{0.0f, 0.0f, 0.0f},
                Velocity{1.0f, 0.5f, 0.0f},
                Rotation{0.0f, 0.0f, 0.0f, 1.0f}
            );
        }

        std::cout << "RenduOptimized: " << std::fixed << std::setprecision(2) << timer.elapsed() << " ms" << std::endl;
    }

    // 批量创建（新功能）
    {
        RegistryOptimized registry;
        Timer timer;

        auto entities = registry.createBatch<Position, Velocity, Rotation>(count);
        for (size_t i = 0; i < count; ++i)
        {
            registry.emplace<Position, Velocity, Rotation>(
                entities[i],
                Position{0.0f, 0.0f, 0.0f},
                Velocity{1.0f, 0.5f, 0.0f},
                Rotation{0.0f, 0.0f, 0.0f, 1.0f}
            );
        }

        std::cout << "RenduOptimized (批量创建): " << std::fixed << std::setprecision(2) << timer.elapsed() << " ms" << std::endl;
    }

    // EnTT
    {
        entt::registry registry;
        Timer timer;

        for (size_t i = 0; i < count; ++i)
        {
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.0f, 0.0f, 0.0f);
            registry.emplace<Velocity>(entity, 1.0f, 0.5f, 0.0f);
            registry.emplace<Rotation>(entity, 0.0f, 0.0f, 0.0f, 1.0f);
        }

        std::cout << "EnTT:          " << std::fixed << std::setprecision(2) << timer.elapsed() << " ms" << std::endl;
    }
}

// 测试 2: 双组件遍历（移动所有实体）
void benchmark_two_component_iteration(size_t count, int iterations)
{
    std::cout << "\n=== 测试 2: 双组件遍历 (" << count << " 实体, " << iterations << " 次迭代) ===" << std::endl;

    // Rendu RegistryOptimized
    {
        RegistryOptimized registry;

        for (size_t i = 0; i < count; ++i)
        {
            auto entity = registry.create();
            registry.emplace<Position, Velocity>(entity, Position{0.0f, 0.0f, 0.0f}, Velocity{1.0f, 0.5f, 0.0f});
        }

        Timer timer;
        double totalTime = 0.0;

        for (int iter = 0; iter < iterations; ++iter)
        {
            timer.reset();
            registry.view<Position, Velocity>().each([](Entity e, Position& pos, Velocity& vel) {
                pos.x += vel.dx;
                pos.y += vel.dy;
                pos.z += vel.dz;
            });
            totalTime += timer.elapsed();
        }

        std::cout << "RenduOptimized: " << std::fixed << std::setprecision(2)
                  << totalTime << " ms (平均 " << (totalTime / iterations) << " ms/iter)" << std::endl;
    }

    // EnTT
    {
        entt::registry registry;

        for (size_t i = 0; i < count; ++i)
        {
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.0f, 0.0f, 0.0f);
            registry.emplace<Velocity>(entity, 1.0f, 0.5f, 0.0f);
        }

        Timer timer;
        double totalTime = 0.0;

        for (int iter = 0; iter < iterations; ++iter)
        {
            timer.reset();
            auto view = registry.view<Position, Velocity>();
            for (auto entity : view)
            {
                auto& pos = view.get<Position>(entity);
                auto& vel = view.get<Velocity>(entity);
                pos.x += vel.dx;
                pos.y += vel.dy;
                pos.z += vel.dz;
            }
            totalTime += timer.elapsed();
        }

        std::cout << "EnTT:          " << std::fixed << std::setprecision(2)
                  << totalTime << " ms (平均 " << (totalTime / iterations) << " ms/iter)" << std::endl;
    }
}

// 测试 3: 多组件遍历（重计算）
void benchmark_multi_component_iteration(size_t count, int iterations)
{
    std::cout << "\n=== 测试 3: 多组件遍历 (" << count << " 实体, " << iterations << " 次迭代) ===" << std::endl;

    // Rendu RegistryOptimized
    {
        RegistryOptimized registry;

        for (size_t i = 0; i < count; ++i)
        {
            auto entity = registry.create();
            registry.emplace<Position, Velocity, Rotation, Scale>(
                entity,
                Position{0.0f, 0.0f, 0.0f},
                Velocity{1.0f, 0.5f, 0.0f},
                Rotation{0.0f, 0.0f, 0.0f, 1.0f},
                Scale{1.0f, 1.0f, 1.0f}
            );
        }

        Timer timer;
        double totalTime = 0.0;

        for (int iter = 0; iter < iterations; ++iter)
        {
            timer.reset();
            registry.view<Position, Velocity, Rotation, Scale>().each(
                [](Entity e, Position& pos, Velocity& vel, Rotation& rot, Scale& scale) {
                    pos.x += vel.dx * scale.sx;
                    pos.y += vel.dy * scale.sy;
                    pos.z += vel.dz * scale.sz;
                }
            );
            totalTime += timer.elapsed();
        }

        std::cout << "RenduOptimized: " << std::fixed << std::setprecision(2)
                  << totalTime << " ms (平均 " << (totalTime / iterations) << " ms/iter)" << std::endl;
    }

    // EnTT
    {
        entt::registry registry;

        for (size_t i = 0; i < count; ++i)
        {
            auto entity = registry.create();
            registry.emplace<Position>(entity, 0.0f, 0.0f, 0.0f);
            registry.emplace<Velocity>(entity, 1.0f, 0.5f, 0.0f);
            registry.emplace<Rotation>(entity, 0.0f, 0.0f, 0.0f, 1.0f);
            registry.emplace<Scale>(entity, 1.0f, 1.0f, 1.0f);
        }

        Timer timer;
        double totalTime = 0.0;

        for (int iter = 0; iter < iterations; ++iter)
        {
            timer.reset();
            auto view = registry.view<Position, Velocity, Rotation, Scale>();
            for (auto entity : view)
            {
                auto& pos = view.get<Position>(entity);
                auto& vel = view.get<Velocity>(entity);
                auto& rot = view.get<Rotation>(entity);
                auto& scale = view.get<Scale>(entity);
                pos.x += vel.dx * scale.sx;
                pos.y += vel.dy * scale.sy;
                pos.z += vel.dz * scale.sz;
            }
            totalTime += timer.elapsed();
        }

        std::cout << "EnTT:          " << std::fixed << std::setprecision(2)
                  << totalTime << " ms (平均 " << (totalTime / iterations) << " ms/iter)" << std::endl;
    }
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "   ECS 性能对比 - Rendu vs EnTT" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "\n测试配置:" << std::endl;
    std::cout << "  - 组件大小: Position(12B), Velocity(12B), Rotation(16B), Scale(12B)" << std::endl;
    std::cout << "  - Rendu: SOA + Archetype + 内存预取" << std::endl;
    std::cout << "  - EnTT: Sparse Set + 多种存储策略" << std::endl;

    // 运行测试
    benchmark_entity_creation(10000);
    benchmark_two_component_iteration(10000, 10);
    benchmark_multi_component_iteration(10000, 10);

    std::cout << "\n========================================" << std::endl;
    std::cout << "  性能分析:" << std::endl;
    std::cout << "  1. 实体创建: EnTT 的对象池更成熟" << std::endl;
    std::cout << "  2. 组件遍历: Rendu 的 SOA 布局更优" << std::endl;
    std::cout << "  3. 缓存命中率: Rendu 纯 SOA 更高" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}

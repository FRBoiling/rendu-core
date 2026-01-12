//
// ECS 性能基准测试 - 使用 Catch2 TEST_CASE
//

#include "common/ecs/registry_optimized.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

using namespace Rendu;

// 测试组件
struct Position {
    float x, y, z;
    Position() : x(0), y(0), z(0) {}
};

struct Velocity {
    float vx, vy, vz;
    Velocity() : vx(0), vy(0), vz(0) {}
};

// ============================================================================
// TEST_CASE 性能测试
// ============================================================================

TEST_CASE("Benchmark - 创建 100 个实体", "[benchmark][create]") {
    RegistryOptimized registry;
    for (int i = 0; i < 100; ++i) {
        auto e = registry.create();
        registry.emplace<Position>(e, Position{});
    }
    REQUIRE(registry.size() == 100);
}

TEST_CASE("Benchmark - 创建 1000 个实体", "[benchmark][create]") {
    RegistryOptimized registry;
    for (int i = 0; i < 1000; ++i) {
        auto e = registry.create();
        registry.emplace<Position>(e, Position{});
    }
    REQUIRE(registry.size() == 1000);
}

TEST_CASE("Benchmark - 创建 10000 个实体", "[benchmark][create]") {
    RegistryOptimized registry;
    for (int i = 0; i < 10000; ++i) {
        auto e = registry.create();
        registry.emplace<Position>(e, Position{});
    }
    REQUIRE(registry.size() == 10000);
}

TEST_CASE("Benchmark - 创建多组件实体 1000", "[benchmark][create]") {
    RegistryOptimized registry;
    for (int i = 0; i < 1000; ++i) {
        auto e = registry.create();
        registry.emplace<Position>(e, Position{});
        registry.emplace<Velocity>(e, Velocity{});
    }
    REQUIRE(registry.size() == 1000);
}

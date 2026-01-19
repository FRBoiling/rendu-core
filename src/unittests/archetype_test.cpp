//
// Archetype 存储系统单元测试
//
// 测试标签说明:
// [archetype][basic]   - Archetype 基础操作测试
// [archetype][batch]   - 批量操作测试
// [archetype][memory]  - 内存布局测试
// [archetype][edge]    - 边界情况测试
//

#include "common/ecs/archetype.h"
#include <catch2/catch_test_macros.hpp>

using namespace Rendu;
// 测试组件
struct Position {
    float x, y;
    Position(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Velocity {
    float vx, vy;
    Velocity(float vx = 0, float vy = 0) : vx(vx), vy(vy) {}
};

struct Health {
    int value;
    Health(int value = 100) : value(value) {}
};

TEST_CASE("Archetype - 基础操作", "[archetype][basic]") {
    Archetype<Position, Velocity> archetype;

    SECTION("添加实体") {
        Entity e(1);
        Position pos{10.0f, 20.0f};
        Velocity vel{0.5f, 0.5f};

        archetype.emplace(1, pos, vel);

        REQUIRE(archetype.size() == 1);
    }

    SECTION("移除实体") {
        Entity e1(1);
        Entity e2(2);

        archetype.emplace(1, Position{0, 0}, Velocity{0, 0});
        archetype.emplace(2, Position{1, 1}, Velocity{1, 1});

        REQUIRE(archetype.size() == 2);

        archetype.remove(1);
        REQUIRE(archetype.size() == 1);
    }
}

TEST_CASE("Archetype - 遍历操作", "[archetype][batch]") {
    SECTION("遍历组件") {
        Archetype<Position, Velocity> archetype;
        const int N = 50;

        for (int i = 0; i < N; ++i) {
            archetype.emplace(i, Position{static_cast<float>(i), 0}, Velocity{0, 0});
        }

        int count = 0;
        float sum = 0.0f;

        archetype.each([&](Entity e, Position& p, Velocity& v) {
            count++;
            sum += p.x;
        });

        REQUIRE(count == N);
        // 0 + 1 + 2 + ... + 49 = 1225
        REQUIRE(sum == 1225.0f);
    }
}

TEST_CASE("Archetype - 内存布局", "[archetype][memory]") {
    SECTION("容量管理") {
        Archetype<Position, Velocity> archetype;

        archetype.reserve(100);

        REQUIRE(archetype.capacity() >= 100);
    }

    SECTION("组件连续存储") {
        Archetype<Position, Velocity> archetype;

        for (int i = 0; i < 5; ++i) {
            archetype.emplace(i, Position{static_cast<float>(i), static_cast<float>(i)}, Velocity{0, 0});
        }

        int count = 0;
        archetype.each([&](Entity e, Position& p, Velocity& v) {
            count++;
        });

        REQUIRE(count == 5);
    }
}

TEST_CASE("Archetype - 多组件类型", "[archetype][edge]") {
    SECTION("三组件类型") {
        Archetype<Position, Velocity, Health> archetype;

        archetype.emplace(1, Position{0, 0}, Velocity{1, 1}, Health{100});
        archetype.emplace(2, Position{1, 1}, Velocity{2, 2}, Health{200});

        REQUIRE(archetype.size() == 2);
    }

    SECTION("双组件类型") {
        Archetype<Position, Velocity> archetype;

        archetype.emplace(1, Position{0, 0}, Velocity{1, 1});
        archetype.emplace(2, Position{1, 1}, Velocity{2, 2});

        REQUIRE(archetype.size() == 2);
    }
}

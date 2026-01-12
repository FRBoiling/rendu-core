//
// Archetype 单元测试
// 使用 Catch2 框架
//

#include "common/ecs/archetype.h"

#include <iostream>
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
        std::cout << "Starting 组件连续存储 test" << std::endl;
        Archetype<Position, Velocity> archetype;

        std::cout << "Adding entities..." << std::endl;
        for (int i = 0; i < 5; ++i) {
            std::cout << "  Adding entity " << i << std::endl;
            archetype.emplace(i, Position{static_cast<float>(i), static_cast<float>(i)}, Velocity{0, 0});
        }

        std::cout << "Size: " << archetype.size() << std::endl;
        std::cout << "Starting each() iteration..." << std::endl;

        int count = 0;
        archetype.each([&](Entity e, Position& p, Velocity& v) {
            count++;
            std::cout << "  Entity " << e.value() << " processed" << std::endl;
            if (count > 10) {
                std::cerr << "Too many iterations!" << std::endl;
                exit(1);
            }
        });

        std::cout << "Final count: " << count << std::endl;
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

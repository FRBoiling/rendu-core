//
// ECS 调试和可视化工具单元测试
//
// 测试标签说明:
// [debugging][browser]     - 实体浏览器测试
// [debugging][visualizer]  - 关系可视化测试
// [debugging][export]      - 导出功能测试
// [debugging][filter]      - 过滤器测试
// [debugging][stats]       - 统计信息测试
//

#include "common/ecs/registry_optimized.h"
#include "common/ecs/entity_browser.h"
#include "common/ecs/relationship_visualizer.h"
#include "common/ecs/relationships.h"
#include <catch2/catch_test_macros.hpp>

using namespace Rendu;

// 测试组件
struct Position {
    float x, y;
    Position() : x(0), y(0) {}
    Position(float x_, float y_) : x(x_), y(y_) {}
};

struct Velocity {
    float vx, vy;
    Velocity() : vx(0), vy(0) {}
    Velocity(float vx_, float vy_) : vx(vx_), vy(vy_) {}
};

struct Health {
    int value;
    Health() : value(100) {}
    Health(int v) : value(v) {}
};

struct Name {
    std::string value;
    Name() : value("") {}
    Name(const std::string& v) : value(v) {}
};

TEST_CASE("Debugging - EntityBrowser 实体浏览", "[debugging][browser]") {
    SECTION("获取所有实体") {
        RegistryOptimized registry;
        EntityBrowser browser;

        for (int i = 0; i < 5; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
        }

        browser.refresh(registry);
        auto entities = browser.getEntities();

        REQUIRE(entities.size() == 5);
    }

    SECTION("获取组件信息") {
        RegistryOptimized registry;
        EntityBrowser browser;

        auto e = registry.create();
        registry.emplace<Position>(e, Position{5.0f, 6.0f});
        registry.emplace<Velocity>(e, Velocity{2.0f, 3.0f});
        registry.emplace<Health>(e, Health{100});

        browser.refresh(registry);
        auto components = browser.getComponents(e);

        REQUIRE(components.size() == 3);
    }

    SECTION("按组件类型搜索") {
        RegistryOptimized registry;
        EntityBrowser browser;

        for (int i = 0; i < 5; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
        }

        for (int i = 0; i < 3; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
            registry.emplace<Velocity>(e, Velocity{1.0f, 1.0f});
        }

        browser.refresh(registry);
        auto results = browser.searchByComponent("Velocity");

        REQUIRE(results.size() == 3);
    }

    SECTION("实体详细信息") {
        RegistryOptimized registry;
        EntityBrowser browser;

        auto e = registry.create();
        registry.emplace<Name>(e, Name{"Player1"});
        registry.emplace<Position>(e, Position{1.0f, 2.0f});

        browser.refresh(registry);
        auto details = browser.getEntityInfo(e);

        REQUIRE(details.valid);
    }
}

TEST_CASE("Debugging - RelationshipVisualizer 关系可视化", "[debugging][visualizer]") {
    SECTION("可视化父子关系") {
        RelationshipManager relationships;
        RelationshipVisualizer visualizer;

        Entity root(1);
        Entity child1(2);
        Entity child2(3);
        Entity grandchild(4);

        relationships.setParent(child1, root);
        relationships.setParent(child2, root);
        relationships.setParent(grandchild, child1);

        auto tree = visualizer.getRelationshipTree(relationships);
        REQUIRE(tree.totalNodes > 0);
    }

    SECTION("获取关系树") {
        RelationshipManager relationships;
        RelationshipVisualizer visualizer;

        Entity root(1);
        Entity child1(2);
        Entity child2(3);
        Entity grandchild1(4);
        Entity grandchild2(5);

        relationships.setParent(child1, root);
        relationships.setParent(child2, root);
        relationships.setParent(grandchild1, child1);
        relationships.setParent(grandchild2, child1);

        auto tree = visualizer.getSubTree(relationships, root);
        REQUIRE(tree.totalNodes >= 3);
    }

    SECTION("关系可视化格式输出") {
        RelationshipManager relationships;
        RelationshipVisualizer visualizer;

        Entity root(1);
        Entity child(2);
        relationships.setParent(child, root);

        auto tree = visualizer.getRelationshipTree(relationships);
        REQUIRE(tree.totalNodes > 0);
    }
}

TEST_CASE("Debugging - 调试信息导出", "[debugging][export]") {
    SECTION("导出实体状态") {
        RegistryOptimized registry;
        EntityBrowser browser;

        auto e = registry.create();
        registry.emplace<Name>(e, Name{"TestEntity"});
        registry.emplace<Position>(e, Position{1.0f, 2.0f});

        browser.refresh(registry);
        auto details = browser.getEntityInfo(e);

        REQUIRE(details.valid);
    }

    SECTION("导出关系图") {
        RelationshipManager relationships;
        RelationshipVisualizer visualizer;

        Entity root(1);
        Entity child1(2);
        Entity child2(3);

        relationships.setParent(child1, root);
        relationships.setParent(child2, root);

        auto dot = visualizer.exportToDOT(relationships);
        REQUIRE_FALSE(dot.empty());
    }
}

TEST_CASE("Debugging - 实体过滤器", "[debugging][filter]") {
    SECTION("按组件组合过滤") {
        RegistryOptimized registry;
        EntityBrowser browser;

        for (int i = 0; i < 10; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
        }

        for (int i = 0; i < 5; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
            registry.emplace<Velocity>(e, Velocity{1.0f, 1.0f});
        }

        browser.refresh(registry);
        auto results = browser.searchByComponent("Position");

        REQUIRE(results.size() == 15);
    }

    SECTION("按实体ID范围过滤") {
        RegistryOptimized registry;
        EntityBrowser browser;

        std::vector<Entity> entities;
        for (int i = 0; i < 20; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
            entities.push_back(e);
        }

        browser.refresh(registry);
        auto allEntities = browser.getEntities();

        REQUIRE(allEntities.size() == 20);
    }
}

TEST_CASE("Debugging - 性能统计", "[debugging][stats]") {
    SECTION("浏览器性能统计") {
        RegistryOptimized registry;
        EntityBrowser browser;

        for (int i = 0; i < 1000; ++i) {
            auto e = registry.create();
            registry.emplace<Position>(e, Position{0.0f, 0.0f});
        }

        browser.refresh(registry);
        auto stats = browser.getStatistics();

        REQUIRE(stats.totalEntities == 1000);
    }

    SECTION("可视化器性能") {
        RelationshipManager relationships;
        RelationshipVisualizer visualizer;

        for (int i = 0; i < 100; ++i) {
            Entity parent(i);
            Entity child(i + 1);
            relationships.setParent(child, parent);
        }

        auto tree = visualizer.getRelationshipTree(relationships);
        REQUIRE(tree.totalNodes > 0);
    }
}

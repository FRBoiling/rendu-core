//
// 关系系统测试
// 使用 Catch2 框架
//

#include "common/ecs/registry_optimized.h"
#include "common/ecs/relationships.h"
#include <catch2/catch_test_macros.hpp>

using namespace Rendu;

// 测试组件
struct Position {
    float x, y, z;
    Position(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

struct Velocity {
    float dx, dy, dz;
    Velocity(float dx = 0, float dy = 0, float dz = 0) : dx(dx), dy(dy), dz(dz) {}
};

struct Name {
    std::string value;
    Name() : value("") {}
    Name(const std::string& v) : value(v) {}
};

TEST_CASE("Relations - 实体关系系统", "[relations][parent_child]") {
    RelationshipManager relationships;

    SECTION("创建父子关系") {
        Entity root(1);
        Entity child1(2);
        Entity child2(3);
        Entity grandchild1(4);

        relationships.setParent(child1, root);
        relationships.setParent(child2, root);
        relationships.setParent(grandchild1, child1);

        REQUIRE(relationships.getChildren(root).size() == 2);
        REQUIRE(relationships.getChildren(child1).size() == 1);
        REQUIRE(relationships.getChildren(child2).size() == 0);
    }

    SECTION("遍历子节点") {
        Entity root(1);
        Entity child1(2);
        Entity child2(3);

        relationships.setParent(child1, root);
        relationships.setParent(child2, root);

        int count = 0;
        relationships.forEachChild(root, [&](Entity child) {
            count++;
        });

        REQUIRE(count == 2);
    }

    SECTION("检查后代关系") {
        Entity root(1);
        Entity child1(2);
        Entity grandchild1(3);

        relationships.setParent(child1, root);
        relationships.setParent(grandchild1, child1);

        REQUIRE(relationships.isDescendant(root, grandchild1));
        REQUIRE_FALSE(relationships.isDescendant(grandchild1, root));
        REQUIRE(relationships.isDescendant(child1, grandchild1));
        REQUIRE_FALSE(relationships.isDescendant(grandchild1, child1));
    }

    SECTION("移除父子关系") {
        Entity root(1);
        Entity child(2);

        relationships.setParent(child, root);
        REQUIRE(relationships.getChildren(root).size() == 1);

        relationships.removeParent(child);
        REQUIRE(relationships.getChildren(root).size() == 0);
    }
}

TEST_CASE("Relations - 关系查询", "[relations][query]") {
    RelationshipManager relationships;

    SECTION("获取父节点") {
        Entity root(1);
        Entity child(2);

        relationships.setParent(child, root);
        auto parent = relationships.getParent(child);

        REQUIRE(parent.value() == 1);
    }

    SECTION("遍历所有后代") {
        Entity root(1);
        Entity child1(2);
        Entity child2(3);
        Entity grandchild1(4);
        Entity grandchild2(5);

        relationships.setParent(child1, root);
        relationships.setParent(child2, root);
        relationships.setParent(grandchild1, child1);
        relationships.setParent(grandchild2, child1);

        int count = 0;
        relationships.forEachDescendant(root, [&](Entity) {
            count++;
        });

        REQUIRE(count == 4);
    }
}

TEST_CASE("Relations - 复杂关系结构", "[relations][complex]") {
    RelationshipManager relationships;

    SECTION("深层嵌套关系") {
        std::vector<Entity> entities;
        for (int i = 0; i < 10; ++i) {
            entities.push_back(Entity(i));
        }

        for (int i = 1; i < 10; ++i) {
            relationships.setParent(entities[i], entities[i - 1]);
        }

        REQUIRE(relationships.isDescendant(entities[0], entities[9]));
        REQUIRE_FALSE(relationships.isDescendant(entities[9], entities[0]));
    }

    SECTION("多子节点关系") {
        Entity root(0);
        std::vector<Entity> children;
        for (int i = 1; i <= 10; ++i) {
            Entity child(i);
            relationships.setParent(child, root);
            children.push_back(child);
        }

        REQUIRE(relationships.getChildren(root).size() == 10);

        int count = 0;
        relationships.forEachChild(root, [&](Entity) {
            count++;
        });
        REQUIRE(count == 10);
    }
}

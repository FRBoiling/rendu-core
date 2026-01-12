//
// ECS 资源管理系统测试
// 使用 Catch2 框架
//

#include "common/ecs/resource.h"
#include "common/ecs/registry_optimized.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

using namespace Rendu;

// 测试资源类型
struct Config {
    int maxEntities;
    float deltaTime;
    std::string name;

    Config() : maxEntities(1000), deltaTime(0.016f), name("default") {}
    Config(int max, float dt, const std::string& n)
        : maxEntities(max), deltaTime(dt), name(n) {}
};

struct AudioSystem {
    bool initialized;
    float volume;

    AudioSystem() : initialized(false), volume(1.0f) {}

    void init() {
        initialized = true;
    }
};

struct TextureCache {
    std::unordered_map<std::string, int> textures;

    void addTexture(const std::string& name, int id) {
        textures[name] = id;
    }

    bool hasTexture(const std::string& name) const {
        return textures.find(name) != textures.end();
    }
};

TEST_CASE("ResourceCache - 基础存储", "[resource][basic]") {
    ResourceCache cache;

    SECTION("添加和获取资源") {
        auto config = std::make_shared<Config>(500, 0.033f, "test_config");

        cache.emplace("main_config", config);

        REQUIRE(cache.contains<Config>("main_config"));
        REQUIRE(cache.get<Config>("main_config") != nullptr);
        REQUIRE(cache.get<Config>("main_config")->maxEntities == 500);
        REQUIRE(cache.get<Config>("main_config")->name == "test_config");
    }

    SECTION("获取不存在的资源") {
        REQUIRE(cache.get<Config>("nonexistent") == nullptr);
        REQUIRE_FALSE(cache.contains<Config>("nonexistent"));
    }

    SECTION("覆盖已存在的资源") {
        auto config1 = std::make_shared<Config>(100, 0.016f, "first");
        auto config2 = std::make_shared<Config>(200, 0.033f, "second");

        cache.emplace("config", config1);
        cache.emplace("config", config2);

        REQUIRE(cache.get<Config>("config")->maxEntities == 200);
        REQUIRE(cache.get<Config>("config")->name == "second");
    }
}

TEST_CASE("Locator - 服务定位器", "[resource][locator]") {
    SECTION("设置和获取服务") {
        auto config = std::make_shared<Config>(99, 0.5f, "locator_test");
        Locator::set<Config>(config);

        REQUIRE(Locator::contains<Config>());
        auto& retrieved = Locator::get<Config>();
        REQUIRE(retrieved.maxEntities == 99);
        REQUIRE(retrieved.name == "locator_test");
    }

    SECTION("获取不存在的服务抛出异常") {
        REQUIRE_FALSE(Locator::contains<AudioSystem>());
        REQUIRE_THROWS(Locator::get<AudioSystem>());
    }

    SECTION("替换服务") {
        auto config1 = std::make_shared<Config>(1, 0.1f, "first");
        auto config2 = std::make_shared<Config>(2, 0.2f, "second");

        Locator::set<Config>(config1);
        Locator::set<Config>(config2);

        auto& retrieved = Locator::get<Config>();
        REQUIRE(retrieved.maxEntities == 2);
        REQUIRE(retrieved.name == "second");
    }

    SECTION("多个独立服务") {
        auto config = std::make_shared<Config>(1000, 0.016f, "config");
        auto audio = std::make_shared<AudioSystem>();

        Locator::set<Config>(config);
        Locator::set<AudioSystem>(audio);

        REQUIRE(Locator::contains<Config>());
        REQUIRE(Locator::contains<AudioSystem>());

        REQUIRE(Locator::get<Config>().maxEntities == 1000);
        REQUIRE_FALSE(Locator::get<AudioSystem>().initialized);
    }
}

TEST_CASE("ResourceCache - 多种资源类型", "[resource][multiple]") {
    ResourceCache cache;

    SECTION("同时管理多种资源") {
        auto config = std::make_shared<Config>(100, 0.016f, "app_config");
        auto audio = std::make_shared<AudioSystem>();
        audio->volume = 0.8f;

        cache.emplace("main_config", config);
        cache.emplace("audio_system", audio);

        REQUIRE(cache.contains<Config>("main_config"));
        REQUIRE(cache.contains<AudioSystem>("audio_system"));

        REQUIRE(cache.get<Config>("main_config")->maxEntities == 100);
        REQUIRE(cache.get<AudioSystem>("audio_system")->volume == 0.8f);
    }

    SECTION("同类型多个资源") {
        auto config1 = std::make_shared<Config>(1, 0.01f, "config1");
        auto config2 = std::make_shared<Config>(2, 0.02f, "config2");
        auto config3 = std::make_shared<Config>(3, 0.03f, "config3");

        cache.emplace("c1", config1);
        cache.emplace("c2", config2);
        cache.emplace("c3", config3);

        REQUIRE(cache.get<Config>("c1")->maxEntities == 1);
        REQUIRE(cache.get<Config>("c2")->maxEntities == 2);
        REQUIRE(cache.get<Config>("c3")->maxEntities == 3);
    }
}

TEST_CASE("Resource - 资源初始化", "[resource][init]") {
    SECTION("资源初始化") {
        auto audio = std::make_shared<AudioSystem>();
        REQUIRE_FALSE(audio->initialized);

        audio->init();
        REQUIRE(audio->initialized);

        Locator::set<AudioSystem>(audio);
        REQUIRE(Locator::get<AudioSystem>().initialized);
    }

    SECTION("获取后初始化") {
        auto audio = std::make_shared<AudioSystem>();
        Locator::set<AudioSystem>(audio);

        REQUIRE_FALSE(Locator::get<AudioSystem>().initialized);

        auto& audioRef = Locator::get<AudioSystem>();
        audioRef.init();

        REQUIRE(Locator::get<AudioSystem>().initialized);
    }
}

TEST_CASE("Resource - 资源与实体交互", "[resource][entity]") {
    SECTION("使用资源配置实体创建") {
        auto config = std::make_shared<Config>(100, 0.033f, "game_config");
        Locator::set<Config>(config);

        RegistryOptimized registry;

        struct Position {
            float x, y;
            Position(float x = 0, float y = 0) : x(x), y(y) {}
        };

        // 创建实体
        for (int i = 0; i < 50; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{0, 0});
        }

        REQUIRE(Locator::contains<Config>());
        REQUIRE(Locator::get<Config>().maxEntities == 100);
    }

    SECTION("资源指导系统行为") {
        ResourceCache cache;
        auto textures = std::make_shared<TextureCache>();
        textures->addTexture("enemy", 10);
        textures->addTexture("player", 20);

        cache.emplace("textures", textures);

        RegistryOptimized registry;

        struct Sprite {
            std::string textureName;
        };

        auto e = registry.create();
        Sprite sprite{"enemy"};
        // 实际项目中会使用资源来验证 textureName
    }
}

TEST_CASE("Resource - 复杂场景", "[resource][complex]") {
    SECTION("资源链式依赖") {
        auto config = std::make_shared<Config>(5000, 0.016f, "production_config");
        auto audio = std::make_shared<AudioSystem>();
        audio->volume = 0.5f;

        auto textures = std::make_shared<TextureCache>();
        textures->addTexture("main_menu_bg", 100);

        ResourceCache cache;
        cache.emplace("config", config);
        cache.emplace("audio", audio);
        cache.emplace("textures", textures);

        REQUIRE(cache.contains<Config>("config"));
        REQUIRE(cache.contains<AudioSystem>("audio"));
        REQUIRE(cache.contains<TextureCache>("textures"));

        REQUIRE(cache.get<Config>("config")->name == "production_config");
        REQUIRE(cache.get<AudioSystem>("audio")->volume == 0.5f);
        REQUIRE(cache.get<TextureCache>("textures")->hasTexture("main_menu_bg"));
    }

    SECTION("资源驱动的系统行为") {
        auto config = std::make_shared<Config>(10, 0.016f, "limited_config");
        Locator::set<Config>(config);

        RegistryOptimized registry;

        struct Position {
            float x, y;
            Position(float x = 0, float y = 0) : x(x), y(y) {}
        };

        // 严格按照配置创建实体
        const int targetCount = config->maxEntities;
        for (int i = 0; i < targetCount; ++i) {
            auto e = registry.create();
            registry.emplaceSingle<Position>(e, Position{static_cast<float>(i), 0});
        }

        // 验证实体数量
        auto view = registry.view<Position>();
        int count = 0;
        view.each([&](Entity e, Position& p) { count++; });

        REQUIRE(count == targetCount);
    }
}

TEST_CASE("Resource - 资源生命周期", "[resource][lifetime]") {
    SECTION("资源在多个系统间共享") {
        auto audio = std::make_shared<AudioSystem>();
        Locator::set<AudioSystem>(audio);

        // 多个系统可以访问同一个资源
        {
            auto& audio1 = Locator::get<AudioSystem>();
            auto& audio2 = Locator::get<AudioSystem>();

            REQUIRE(&audio1 == &audio2); // 同一个对象
        }
    }

    SECTION("ResourceCache 资源独立管理") {
        ResourceCache cache;

        auto config1 = std::make_shared<Config>(1, 0.01f, "config1");
        auto config2 = std::make_shared<Config>(2, 0.02f, "config2");

        cache.emplace("id1", config1);
        cache.emplace("id2", config2);

        REQUIRE(cache.get<Config>("id1")->maxEntities == 1);
        REQUIRE(cache.get<Config>("id2")->maxEntities == 2);

        // 修改一个不应该影响另一个
        cache.get<Config>("id1")->name = "modified";

        REQUIRE(cache.get<Config>("id1")->name == "modified");
        REQUIRE(cache.get<Config>("id2")->name == "config2");
    }
}

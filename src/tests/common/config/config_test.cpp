//**********************************
//  Created by boil on 2026/1/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/config/config.h"

using namespace Rendu;
using namespace Rendu::config;

TEST_CASE("Config: 基本设置和获取", "[config]") {
    Config config;

    SECTION("设置和获取整数值") {
        config.set("port", int64_t(8080));
        auto result = config.get<int64_t>("port");

        REQUIRE(std::holds_alternative<int64_t>(result));
        REQUIRE(std::get<int64_t>(result) == 8080);
    }

    SECTION("设置和获取字符串值") {
        config.set("host", std::string("127.0.0.1"));
        auto result = config.get<std::string>("host");

        REQUIRE(std::holds_alternative<std::string>(result));
        REQUIRE(std::get<std::string>(result) == "127.0.0.1");
    }

    SECTION("设置和获取布尔值") {
        config.set("enabled", true);
        auto result = config.get<bool>("enabled");

        REQUIRE(std::holds_alternative<bool>(result));
        REQUIRE(std::get<bool>(result) == true);
    }

    SECTION("设置和获取浮点数值") {
        config.set("timeout", 3.5);
        auto result = config.get<double>("timeout");

        REQUIRE(std::holds_alternative<double>(result));
        REQUIRE(std::get<double>(result) == 3.5);
    }
}

TEST_CASE("Config: 存在性检查", "[config]") {
    Config config;

    config.set("existing_key", std::string("value"));

    SECTION("存在的键") {
        REQUIRE(config.has("existing_key") == true);
    }

    SECTION("不存在的键") {
        REQUIRE(config.has("non_existing_key") == false);
    }
}

TEST_CASE("Config: 默认值", "[config]") {
    Config config;

    SECTION("不存在的键返回默认值") {
        auto value = config.get_or_default<int64_t>("missing", int64_t(42));
        REQUIRE(value == 42);
    }

    SECTION("存在的键不使用默认值") {
        config.set("key", int64_t(100));
        auto value = config.get_or_default<int64_t>("key", int64_t(42));
        REQUIRE(value == 100);
    }

    SECTION("类型不匹配时返回默认值") {
        config.set("key", std::string("string_value"));
        auto value = config.get_or_default<int64_t>("key", int64_t(42));
        REQUIRE(value == 42);
    }
}

TEST_CASE("Config: 配置合并", "[config]") {
    Config config1;
    Config config2;

    config1.set("key1", std::string("value1"));
    config1.set("key2", std::string("value2"));

    config2.set("key2", std::string("new_value2"));
    config2.set("key3", std::string("value3"));

    SECTION("简单合并") {
        config1.merge(config2);

        REQUIRE(std::get<std::string>(config1.get<std::string>("key1")) == "value1");
        REQUIRE(std::get<std::string>(config1.get<std::string>("key2")) == "new_value2");
        REQUIRE(std::get<std::string>(config1.get<std::string>("key3")) == "value3");
    }

    SECTION("合并到空配置") {
        Config empty;
        empty.merge(config1);

        REQUIRE(std::get<std::string>(empty.get<std::string>("key1")) == "value1");
        REQUIRE(std::get<std::string>(empty.get<std::string>("key2")) == "value2");
    }
}

TEST_CASE("Config: 数组值", "[config]") {
    Config config;

    SECTION("创建和获取整数数组") {
        std::vector<int64_t> array = {1, 2, 3};
        config.set("numbers", array);

        auto result = config.get<std::vector<int64_t>>("numbers");
        REQUIRE(std::holds_alternative<std::vector<int64_t>>(result));
        REQUIRE(std::get<std::vector<int64_t>>(result).size() == 3);
    }
}

TEST_CASE("Config: 键列表", "[config]") {
    Config config;

    config.set("key1", std::string("value1"));
    config.set("key2", std::string("value2"));
    config.set("key3", std::string("value3"));

    SECTION("获取所有顶层键") {
        auto keys = config.keys();

        REQUIRE(keys.size() == 3);
        REQUIRE(std::find(keys.begin(), keys.end(), "key1") != keys.end());
        REQUIRE(std::find(keys.begin(), keys.end(), "key2") != keys.end());
        REQUIRE(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    }
}

TEST_CASE("Config: 清空和大小", "[config]") {
    Config config;

    config.set("key1", std::string("value1"));
    config.set("key2", std::string("value2"));

    SECTION("大小") {
        REQUIRE(config.size() == 2);
        REQUIRE(config.empty() == false);
    }

    SECTION("清空") {
        config.clear();

        REQUIRE(config.size() == 0);
        REQUIRE(config.empty() == true);
    }
}

TEST_CASE("Config: 类型错误处理", "[config]") {
    Config config;

    config.set("port", int64_t(8080));

    SECTION("类型不匹配") {
        auto result = config.get<std::string>("port");
        REQUIRE(std::holds_alternative<Error>(result));
    }
}

TEST_CASE("Config: 嵌套配置 - 点号分隔路径访问", "[config]") {
    Config config;

    SECTION("设置和获取嵌套值") {
        config.set("database.port", int64_t(5432));
        config.set("database.host", std::string("localhost"));
        config.set("database.timeout", 5.5);
        config.set("server.port", int64_t(8080));

        REQUIRE(std::get<int64_t>(config.get<int64_t>("database.port")) == 5432);
        REQUIRE(std::get<std::string>(config.get<std::string>("database.host")) == "localhost");
        REQUIRE(std::get<double>(config.get<double>("database.timeout")) == 5.5);
        REQUIRE(std::get<int64_t>(config.get<int64_t>("server.port")) == 8080);
    }

    SECTION("多层嵌套") {
        config.set("database.credentials.username", std::string("admin"));
        config.set("database.credentials.password", std::string("secret"));
        config.set("database.connection.max_retries", int64_t(3));

        REQUIRE(std::get<std::string>(config.get<std::string>("database.credentials.username")) == "admin");
        REQUIRE(std::get<std::string>(config.get<std::string>("database.credentials.password")) == "secret");
        REQUIRE(std::get<int64_t>(config.get<int64_t>("database.connection.max_retries")) == 3);
    }
}

TEST_CASE("Config: 嵌套配置 - get_sub_config", "[config]") {
    Config config;

    SECTION("获取嵌套配置对象") {
        config.set("database.port", int64_t(5432));
        config.set("database.host", std::string("localhost"));
        config.set("database.credentials.username", std::string("admin"));
        config.set("server.port", int64_t(8080));

        auto db_config = config.get_sub_config("database");
        REQUIRE(db_config.has_value());

        REQUIRE(std::get<int64_t>(db_config->get<int64_t>("port")) == 5432);
        REQUIRE(std::get<std::string>(db_config->get<std::string>("host")) == "localhost");

        auto creds_config = db_config->get_sub_config("credentials");
        REQUIRE(creds_config.has_value());
        REQUIRE(std::get<std::string>(creds_config->get<std::string>("username")) == "admin");
    }

    SECTION("不存在的嵌套配置") {
        auto missing = config.get_sub_config("nonexistent");
        REQUIRE_FALSE(missing.has_value());
    }

    SECTION("非 Config 类型的值") {
        config.set("port", int64_t(8080));
        auto not_config = config.get_sub_config("port");
        REQUIRE_FALSE(not_config.has_value());
    }
}

TEST_CASE("Config: 嵌套配置 - 合并", "[config]") {
    Config config1;
    Config config2;

    SECTION("嵌套配置合并") {
        config1.set("database.port", int64_t(5432));
        config1.set("database.host", std::string("localhost"));

        config2.set("database.timeout", 10.0);
        config2.set("server.port", int64_t(8080));

        config1.merge(config2);

        REQUIRE(std::get<int64_t>(config1.get<int64_t>("database.port")) == 5432);
        REQUIRE(std::get<std::string>(config1.get<std::string>("database.host")) == "localhost");
        REQUIRE(std::get<double>(config1.get<double>("database.timeout")) == 10.0);
        REQUIRE(std::get<int64_t>(config1.get<int64_t>("server.port")) == 8080);
    }

    SECTION("深层嵌套合并") {
        config1.set("database.credentials.username", std::string("admin"));
        config2.set("database.credentials.password", std::string("secret"));

        config1.merge(config2);

        auto db_config = config1.get_sub_config("database");
        REQUIRE(db_config.has_value());

        auto creds_config = db_config->get_sub_config("credentials");
        REQUIRE(creds_config.has_value());

        REQUIRE(std::get<std::string>(creds_config->get<std::string>("username")) == "admin");
        REQUIRE(std::get<std::string>(creds_config->get<std::string>("password")) == "secret");
    }
}

TEST_CASE("Config: 嵌套配置 - 存在性检查", "[config]") {
    Config config;

    SECTION("检查嵌套键存在性") {
        config.set("database.port", int64_t(5432));
        config.set("database.host", std::string("localhost"));

        REQUIRE(config.has("database.port"));
        REQUIRE(config.has("database.host"));
        REQUIRE_FALSE(config.has("database.timeout"));
        REQUIRE_FALSE(config.has("server.port"));
    }

    SECTION("检查多层嵌套键存在性") {
        config.set("database.credentials.username", std::string("admin"));

        REQUIRE(config.has("database.credentials.username"));
        REQUIRE_FALSE(config.has("database.credentials.password"));
        REQUIRE_FALSE(config.has("database.connection.timeout"));
    }
}

TEST_CASE("Config: 嵌套配置 - 覆盖行为", "[config]") {
    Config config;

    SECTION("覆盖嵌套路径上的值") {
        config.set("database.port", int64_t(5432));
        config.set("database", int64_t(42));  // 用非 Config 值覆盖

        // 现在 database 不是 Config 类型，无法访问 port
        REQUIRE_FALSE(config.has("database.port"));
        REQUIRE(std::get<int64_t>(config.get<int64_t>("database")) == 42);
    }

    SECTION("用 Config 覆盖非 Config 值") {
        auto nested = std::make_shared<Config>();
        nested->set("port", int64_t(8080));
        config.set("database", ConfigValue(nested));

        REQUIRE(std::get<int64_t>(config.get<int64_t>("database.port")) == 8080);
    }
}

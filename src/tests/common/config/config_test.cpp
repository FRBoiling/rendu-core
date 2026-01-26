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

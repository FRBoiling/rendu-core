//**********************************
//  Created by boil on 2026/1/26.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include "common/config/loader.h"
#include <fstream>
#include <filesystem>

using namespace Rendu;
using namespace Rendu::config;

// 创建临时测试文件
class TempFile {
public:
    explicit TempFile(const std::string& content) {
        path_ = std::filesystem::temp_directory_path() / ("test_config_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json");

        std::ofstream file(path_);
        file << content;
        file.close();
    }

    ~TempFile() {
        std::error_code ec;
        std::filesystem::remove(path_, ec);
    }

    const std::string& path() const {
        return path_;
    }

private:
    std::string path_;
};

TEST_CASE("JsonLoader: 加载基本配置", "[config][loader]") {
    std::string json_content = R"({
        "port": 8080,
        "host": "localhost",
        "enabled": true,
        "timeout": 3.5
    })";

    TempFile file(json_content);
    JsonLoader loader;
    auto result = loader.load(file.path());

    REQUIRE(std::holds_alternative<Config>(result));

    const Config& config = std::get<Config>(result);

    REQUIRE(config.has("port"));
    REQUIRE(config.has("host"));
    REQUIRE(config.has("enabled"));
    REQUIRE(config.has("timeout"));

    REQUIRE(std::get<int64_t>(config.get<int64_t>("port")) == 8080);
    REQUIRE(std::get<std::string>(config.get<std::string>("host")) == "localhost");
    REQUIRE(std::get<bool>(config.get<bool>("enabled")) == true);
    REQUIRE(std::get<double>(config.get<double>("timeout")) == 3.5);
}

TEST_CASE("JsonLoader: 文件不存在", "[config][loader]") {
    JsonLoader loader;
    auto result = loader.load("nonexistent_file.json");

    REQUIRE(std::holds_alternative<Error>(result));
    REQUIRE(std::get<Error>(result).code() == ErrorCode::NotFound);
}

TEST_CASE("JsonLoader: 空文件", "[config][loader]") {
    TempFile file("");
    JsonLoader loader;
    auto result = loader.load(file.path());

    REQUIRE(std::holds_alternative<Error>(result));
    REQUIRE(std::get<Error>(result).code() == ErrorCode::InvalidArgument);
}

TEST_CASE("JsonLoader: 复杂嵌套结构", "[config][loader]") {
    std::string json_content = R"({
        "app": {
            "name": "TestApp",
            "version": "1.0.0"
        },
        "ports": [8080, 8081, 8082]
    })";

    TempFile file(json_content);
    JsonLoader loader;
    auto result = loader.load(file.path());

    REQUIRE(std::holds_alternative<Config>(result));

    const Config& config = std::get<Config>(result);

    REQUIRE(config.has("app"));
    REQUIRE(config.has("ports"));
}

TEST_CASE("JsonLoader: 嵌套配置 - 数据库配置", "[config][loader]") {
    std::string json_content = R"({
        "database": {
            "host": "localhost",
            "port": 5432,
            "credentials": {
                "username": "admin",
                "password": "secret",
                "timeout": 30.0
            },
            "connection": {
                "max_retries": 3,
                "enabled": true
            }
        },
        "server": {
            "port": 8080,
            "host": "0.0.0.0"
        }
    })";

    TempFile file(json_content);
    JsonLoader loader;
    auto result = loader.load(file.path());

    REQUIRE(std::holds_alternative<Config>(result));
    const Config& config = std::get<Config>(result);

    // 使用点号路径访问嵌套值
    REQUIRE(std::get<std::string>(config.get<std::string>("database.host")) == "localhost");
    REQUIRE(std::get<int64_t>(config.get<int64_t>("database.port")) == 5432);
    REQUIRE(std::get<std::string>(config.get<std::string>("database.credentials.username")) == "admin");
    REQUIRE(std::get<std::string>(config.get<std::string>("database.credentials.password")) == "secret");
    REQUIRE(std::get<double>(config.get<double>("database.credentials.timeout")) == 30.0);
    REQUIRE(std::get<int64_t>(config.get<int64_t>("database.connection.max_retries")) == 3);
    REQUIRE(std::get<bool>(config.get<bool>("database.connection.enabled")) == true);
    REQUIRE(std::get<int64_t>(config.get<int64_t>("server.port")) == 8080);

    // 使用 get_sub_config 获取嵌套配置
    auto db_config = config.get_sub_config("database");
    REQUIRE(db_config.has_value());
    REQUIRE(std::get<std::string>(db_config->get<std::string>("host")) == "localhost");

    auto creds_config = db_config->get_sub_config("credentials");
    REQUIRE(creds_config.has_value());
    REQUIRE(std::get<std::string>(creds_config->get<std::string>("username")) == "admin");
}

TEST_CASE("JsonLoader: 嵌套配置 - 多层嵌套", "[config][loader]") {
    std::string json_content = R"({
        "level1": {
            "level2": {
                "level3": {
                    "level4": {
                        "value": "deep"
                    }
                }
            }
        }
    })";

    TempFile file(json_content);
    JsonLoader loader;
    auto result = loader.load(file.path());

    REQUIRE(std::holds_alternative<Config>(result));
    const Config& config = std::get<Config>(result);

    REQUIRE(std::get<std::string>(config.get<std::string>("level1.level2.level3.level4.value")) == "deep");

    auto level1 = config.get_sub_config("level1");
    REQUIRE(level1.has_value());

    auto level2 = level1->get_sub_config("level2");
    REQUIRE(level2.has_value());

    auto level3 = level2->get_sub_config("level3");
    REQUIRE(level3.has_value());

    auto level4 = level3->get_sub_config("level4");
    REQUIRE(level4.has_value());
    REQUIRE(std::get<std::string>(level4->get<std::string>("value")) == "deep");
}

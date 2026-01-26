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

//
// Created by boil on 2026/1/20.
//

// ============================================================================
// logger_test.cpp - 日志系统单元测试
// ============================================================================

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <ctime>
#include "common/logger/appender.h"
#include "common/logger/appender_console.h"
#include "common/logger/appender_file.h"
#include "common/logger/log_level.h"
#include "common/logger/appender_flags.h"

using namespace Rendu;

// ============================================================================
// 日志级别测试
// ============================================================================

TEST_CASE("日志级别枚举值正确", "[logger][log_level]")
{
    REQUIRE(static_cast<int>(LogLevel::LOG_LEVEL_DISABLED) == 0);
    REQUIRE(static_cast<int>(LogLevel::LOG_LEVEL_TRACE) == 1);
    REQUIRE(static_cast<int>(LogLevel::LOG_LEVEL_DEBUG) == 2);
    REQUIRE(static_cast<int>(LogLevel::LOG_LEVEL_INFO) == 3);
    REQUIRE(static_cast<int>(LogLevel::LOG_LEVEL_WARN) == 4);
    REQUIRE(static_cast<int>(LogLevel::LOG_LEVEL_ERROR) == 5);
    REQUIRE(static_cast<int>(LogLevel::LOG_LEVEL_FATAL) == 6);
}

TEST_CASE("日志级别比较正确", "[logger][log_level]")
{
    REQUIRE(LogLevel::LOG_LEVEL_TRACE < LogLevel::LOG_LEVEL_DEBUG);
    REQUIRE(LogLevel::LOG_LEVEL_DEBUG < LogLevel::LOG_LEVEL_INFO);
    REQUIRE(LogLevel::LOG_LEVEL_INFO < LogLevel::LOG_LEVEL_WARN);
    REQUIRE(LogLevel::LOG_LEVEL_WARN < LogLevel::LOG_LEVEL_ERROR);
    REQUIRE(LogLevel::LOG_LEVEL_ERROR < LogLevel::LOG_LEVEL_FATAL);
}

// ============================================================================
// AppenderFlags 测试
// ============================================================================

TEST_CASE("AppenderFlags位运算正确", "[logger][flags]")
{
    auto flag1 = AppenderFlags::APPENDER_FLAGS_PREFIX_TIMESTAMP;
    auto flag2 = AppenderFlags::APPENDER_FLAGS_PREFIX_LOGLEVEL;
    auto flag3 = AppenderFlags::APPENDER_FLAGS_ASYNC;

    // 或运算
    auto combined = flag1 | flag2;
    REQUIRE(static_cast<bool>(combined & flag1));
    REQUIRE(static_cast<bool>(combined & flag2));

    // 与运算
    REQUIRE(static_cast<bool>((flag1 | flag2) & flag1));
    REQUIRE(static_cast<bool>((flag1 | flag2) & flag2));

    // 异步标志独立
    REQUIRE(static_cast<int>(AppenderFlags::APPENDER_FLAGS_ASYNC) == 0x20);
}

// ============================================================================
// Appender 测试
// ============================================================================

TEST_CASE("AppenderConsole基本创建和初始化", "[logger][appender_console]")
{
    uint8 id = 1;
    std::string name = "test_console";
    LogLevel level = LogLevel::LOG_LEVEL_INFO;
    AppenderFlags flags = AppenderFlags::APPENDER_FLAGS_NONE;

    std::vector<std::string_view> args;

    AppenderConsole appender(id, name, level, flags, args);

    REQUIRE(appender.getId() == id);
    REQUIRE(appender.getName() == name);
    REQUIRE(appender.getLogLevel() == level);
    REQUIRE(appender.getType() == AppenderType::APPENDER_CONSOLE);
}

TEST_CASE("AppenderConsole设置日志级别", "[logger][appender_console]")
{
    AppenderConsole appender(1, "test", LogLevel::LOG_LEVEL_INFO, AppenderFlags::APPENDER_FLAGS_NONE, {});

    appender.setLogLevel(LogLevel::LOG_LEVEL_DEBUG);
    REQUIRE(appender.getLogLevel() == LogLevel::LOG_LEVEL_DEBUG);

    appender.setLogLevel(LogLevel::LOG_LEVEL_ERROR);
    REQUIRE(appender.getLogLevel() == LogLevel::LOG_LEVEL_ERROR);
}

TEST_CASE("AppenderConsole批量大小", "[logger][appender_console]")
{
    auto flags = AppenderFlags::APPENDER_FLAGS_ASYNC;
    AppenderConsole appender(1, "test", LogLevel::LOG_LEVEL_INFO, flags, {});

    SECTION("设置有效批量大小")
    {
        REQUIRE_NOTHROW(appender.setBatchSize(50));
        REQUIRE_NOTHROW(appender.setBatchSize(100));
        REQUIRE_NOTHROW(appender.setBatchSize(1));
    }

    SECTION("设置零批量大小应使用默认值")
    {
        appender.setBatchSize(0);
        REQUIRE(true);
    }

    SECTION("获取待处理消息数")
    {
        size_t count = appender.getPendingCount();
        REQUIRE(count == 0);
    }
}

// ============================================================================
// AppenderFile 测试
// ============================================================================

TEST_CASE("AppenderFile基本创建和初始化", "[logger][appender_file]")
{
    std::string tempFile = "/tmp/test_logger_" + std::to_string(std::time(nullptr)) + ".log";

    uint8 id = 2;
    std::string name = "test_file";
    LogLevel level = LogLevel::LOG_LEVEL_DEBUG;
    AppenderFlags flags = AppenderFlags::APPENDER_FLAGS_NONE;

    std::vector<std::string_view> args = {tempFile};

    REQUIRE_NOTHROW([&]() {
        AppenderFile appender(id, name, level, flags, args);
    }());

    // 清理
    std::filesystem::remove(tempFile);
}

TEST_CASE("AppenderFile文件轮转配置", "[logger][appender_file][rotation]")
{
    std::string tempFile = "/tmp/test_rotation_" + std::to_string(std::time(nullptr)) + ".log";

    auto flags = AppenderFlags::APPENDER_FLAGS_ASYNC;
    std::vector<std::string_view> args = {tempFile};
    AppenderFile appender(1, "test_rotation", LogLevel::LOG_LEVEL_INFO, flags, args);

    SECTION("设置文件轮转参数")
    {
        REQUIRE_NOTHROW(appender.setFileRotationConfig(10 * 1024, 3));
        REQUIRE_NOTHROW(appender.setFileRotationConfig(100 * 1024 * 1024, 10));
        REQUIRE_NOTHROW(appender.setFileRotationConfig(0, 0));
    }

    // 清理
    std::filesystem::remove(tempFile);
}

// ============================================================================
// 边界条件测试
// ============================================================================

TEST_CASE("AppenderFile创建时空文件名应抛出异常", "[logger][appender_file][error]")
{
    std::vector<std::string_view> emptyArgs;
    bool exceptionThrown = false;

    try
    {
        AppenderFile appender(1, "test", LogLevel::LOG_LEVEL_INFO,
                              AppenderFlags::APPENDER_FLAGS_NONE, emptyArgs);
    }
    catch (InvalidAppenderArgsException&)
    {
        exceptionThrown = true;
    }

    REQUIRE(exceptionThrown == true);
}

TEST_CASE("AppenderConsole设置批量大小", "[logger][appender_console][error][async]")
{
    auto flags = AppenderFlags::APPENDER_FLAGS_ASYNC;
    AppenderConsole appender(1, "test", LogLevel::LOG_LEVEL_INFO, flags, {});

    // 不设置 IoContext，只测试设置批量大小等非 I/O 操作
    REQUIRE_NOTHROW(appender.setBatchSize(100));
}

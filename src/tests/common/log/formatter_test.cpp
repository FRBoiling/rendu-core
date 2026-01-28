//**********************************
//  Created by boil on 2026/01/28.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <common/log/formatter.h>
#include <sstream>

using namespace Rendu::log;

TEST_CASE("DefaultFormatter format", "[log][formatter]") {
    DefaultFormatter formatter;

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test_logger";
    msg.message = "test message";
    msg.timestamp = "2026-01-22 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();

    std::string formatted = formatter.format(msg);

    REQUIRE(formatted.find("[2026-01-22 12:00:00.000]") != std::string::npos);
    REQUIRE(formatted.find("[INFO ]") != std::string::npos);
    REQUIRE(formatted.find("[test_logger]") != std::string::npos);
    REQUIRE(formatted.find("test message") != std::string::npos);
}

TEST_CASE("DefaultFormatter with fields", "[log][formatter]") {
    DefaultFormatter formatter;

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test";
    msg.message = "event";
    msg.timestamp = "2026-01-22 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();
    msg.fields["key1"] = "value1";
    msg.fields["key2"] = "value2";

    std::string formatted = formatter.format(msg);

    REQUIRE(formatted.find("[key1=value1, key2=value2]") != std::string::npos);
}

TEST_CASE("DefaultFormatter level strings", "[log][formatter]") {
    DefaultFormatter formatter;

    LogMessage msg;
    msg.logger_name = "test";
    msg.message = "msg";
    msg.timestamp = "2026-01-22 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();

    msg.level = Level::Trace;
    REQUIRE(formatter.format(msg).find("[TRACE]") != std::string::npos);

    msg.level = Level::Debug;
    REQUIRE(formatter.format(msg).find("[DEBUG]") != std::string::npos);

    msg.level = Level::Info;
    REQUIRE(formatter.format(msg).find("[INFO ]") != std::string::npos);

    msg.level = Level::Warn;
    REQUIRE(formatter.format(msg).find("[WARN ]") != std::string::npos);

    msg.level = Level::Error;
    REQUIRE(formatter.format(msg).find("[ERROR]") != std::string::npos);

    msg.level = Level::Critical;
    REQUIRE(formatter.format(msg).find("[CRITICAL]") != std::string::npos);
}

TEST_CASE("PatternFormatter basic", "[log][formatter]") {
    PatternFormatter formatter("%t|%l|%n|%m");

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test";
    msg.message = "hello";
    msg.timestamp = "2024-01-01 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();

    std::string result = formatter.format(msg);

    REQUIRE(result == "2024-01-01 12:00:00.000|INFO|test|hello");
}

TEST_CASE("PatternFormatter all placeholders", "[log][formatter]") {
    PatternFormatter formatter("[%t] [%l] [%i] %n: %m");

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test_logger";
    msg.message = "test message";
    msg.timestamp = "2024-01-01 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();

    std::string result = formatter.format(msg);

    REQUIRE(result.find("[2024-01-01 12:00:00.000]") != std::string::npos);
    REQUIRE(result.find("[INFO]") != std::string::npos);
    REQUIRE(result.find("[") != std::string::npos);
    REQUIRE(result.find("]") != std::string::npos);
    REQUIRE(result.find("test_logger:") != std::string::npos);
    REQUIRE(result.find("test message") != std::string::npos);
}

TEST_CASE("PatternFormatter percent escape", "[log][formatter]") {
    PatternFormatter formatter("%%");

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test";
    msg.message = "msg";
    msg.timestamp = "2024-01-01 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();

    std::string result = formatter.format(msg);
    REQUIRE(result == "%");
}

TEST_CASE("PatternFormatter with fields", "[log][formatter]") {
    PatternFormatter formatter("%m [%f]");

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test";
    msg.message = "event";
    msg.timestamp = "2024-01-01 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();
    msg.fields["key1"] = "value1";
    msg.fields["key2"] = "value2";

    std::string result = formatter.format(msg);

    REQUIRE(result == "event [key1=value1 key2=value2]");
}

TEST_CASE("PatternFormatter with empty fields", "[log][formatter]") {
    PatternFormatter formatter("%m [%f]");

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test";
    msg.message = "event";
    msg.timestamp = "2024-01-01 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();
    msg.fields.clear();

    std::string result = formatter.format(msg);

    REQUIRE(result == "event []");
}

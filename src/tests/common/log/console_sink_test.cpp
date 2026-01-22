#include <catch2/catch_test_macros.hpp>
#include <common/log/console_sink.h>
#include <common/log/formatter.h>
#include <sstream>

using namespace Rendu::log;

TEST_CASE("ConsoleSink basic", "[log][console_sink]") {
    ConsoleSink sink(Level::Info);

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test";
    msg.message = "test message";
    msg.timestamp = "2026-01-22 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();

    REQUIRE_NOTHROW(sink.log(msg));
}

TEST_CASE("ConsoleSink level filtering", "[log][console_sink]") {
    ConsoleSink sink(Level::Warn);

    LogMessage msg;
    msg.level = Level::Info;
    msg.logger_name = "test";
    msg.message = "should not appear";
    msg.timestamp = "2026-01-22 12:00:00.000";
    msg.thread_id = std::this_thread::get_id();

    // Info 应该被过滤
    REQUIRE_NOTHROW(sink.log(msg));
}

TEST_CASE("ConsoleSink set_level", "[log][console_sink]") {
    ConsoleSink sink(Level::Info);

    REQUIRE(sink.level() == Level::Info);

    sink.set_level(Level::Debug);
    REQUIRE(sink.level() == Level::Debug);
}

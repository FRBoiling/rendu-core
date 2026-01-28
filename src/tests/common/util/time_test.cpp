//**********************************
//  Created by boil on 2026/01/28.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <common/util/time.h>
#include <common/define.h>
#include <thread>
#include <chrono>
#include <cmath>

using namespace Rendu::time;

TEST_CASE("获取当前时间（毫秒）", "[util][time]") {
    auto t1 = now_ms();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto t2 = now_ms();
    REQUIRE(t2 - t1 >= 10);
    REQUIRE(t2 - t1 < 100); // Should be close to 10ms
}

TEST_CASE("获取当前时间（微秒）", "[util][time]") {
    auto t1 = now_us();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto t2 = now_us();
    auto diff_ms = (t2 - t1) / 1000;
    REQUIRE(diff_ms >= 10);
    REQUIRE(diff_ms < 100);
}

TEST_CASE("格式化时间", "[util][time]") {
    auto timestamp = now_ms();
    std::string formatted = format_time(timestamp);
    REQUIRE(formatted.length() == 19); // "YYYY-MM-DD HH:MM:SS"

    auto parsed_timestamp = parse_time(formatted);
    auto diff = std::abs(timestamp - parsed_timestamp);
    REQUIRE(diff < 1000); // Less than 1 second difference
}

TEST_CASE("解析时间", "[util][time]") {
    auto timestamp = parse_time("2024-01-15 10:30:45");
    auto formatted = format_time(timestamp);
    REQUIRE(formatted == "2024-01-15 10:30:45");
}

TEST_CASE("休眠毫秒", "[util][time]") {
    auto t1 = now_ms();
    sleep_ms(50);
    auto t2 = now_ms();
    REQUIRE(t2 - t1 >= 45); // Allow some tolerance
    REQUIRE(t2 - t1 < 100);
}

TEST_CASE("秒表：获取经过时间（毫秒）", "[util][time]") {
    Stopwatch stopwatch;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(stopwatch.elapsed_ms() >= 45);
    REQUIRE(stopwatch.elapsed_ms() < 100);
}

TEST_CASE("秒表：获取经过时间（微秒）", "[util][time]") {
    Stopwatch stopwatch;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    auto elapsed_us = stopwatch.elapsed_us();
    REQUIRE(elapsed_us >= 45000);
    REQUIRE(elapsed_us < 100000);
}

TEST_CASE("秒表：重置", "[util][time]") {
    Stopwatch stopwatch;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(stopwatch.elapsed_ms() >= 45);

    stopwatch.reset();
    REQUIRE(stopwatch.elapsed_ms() < 10); // Should be close to 0

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    REQUIRE(stopwatch.elapsed_ms() >= 15);
    REQUIRE(stopwatch.elapsed_ms() < 50);
}

#include <catch2/catch_test_macros.hpp>
#include <common/util/time.h>
#include <common/define.h>
#include <thread>
#include <chrono>

using namespace Rendu::time;

TEST_CASE("time now_ms", "[util][time]") {
    auto t1 = now_ms();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto t2 = now_ms();
    REQUIRE(t2 - t1 >= 10);
    REQUIRE(t2 - t1 < 100); // Should be close to 10ms
}

TEST_CASE("time now_us", "[util][time]") {
    auto t1 = now_us();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto t2 = now_us();
    auto diff_ms = (t2 - t1) / 1000;
    REQUIRE(diff_ms >= 10);
    REQUIRE(diff_ms < 100);
}

TEST_CASE("time format_time", "[util][time]") {
    auto timestamp = now_ms();
    std::string formatted = format_time(timestamp);
    REQUIRE(formatted.length() == 19); // "YYYY-MM-DD HH:MM:SS"

    auto parsed_timestamp = parse_time(formatted);
    auto diff = std::abs(timestamp - parsed_timestamp);
    REQUIRE(diff < 1000); // Less than 1 second difference
}

TEST_CASE("time parse_time", "[util][time]") {
    auto timestamp = parse_time("2024-01-15 10:30:45");
    auto formatted = format_time(timestamp);
    REQUIRE(formatted == "2024-01-15 10:30:45");
}

TEST_CASE("time sleep_ms", "[util][time]") {
    auto t1 = now_ms();
    sleep_ms(50);
    auto t2 = now_ms();
    REQUIRE(t2 - t1 >= 45); // Allow some tolerance
    REQUIRE(t2 - t1 < 100);
}

TEST_CASE("Timer elapsed_ms", "[util][time]") {
    Timer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(timer.elapsed_ms() >= 45);
    REQUIRE(timer.elapsed_ms() < 100);
}

TEST_CASE("Timer elapsed_us", "[util][time]") {
    Timer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    auto elapsed_us = timer.elapsed_us();
    REQUIRE(elapsed_us >= 45000);
    REQUIRE(elapsed_us < 100000);
}

TEST_CASE("Timer reset", "[util][time]") {
    Timer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(timer.elapsed_ms() >= 45);

    timer.reset();
    REQUIRE(timer.elapsed_ms() < 10); // Should be close to 0

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    REQUIRE(timer.elapsed_ms() >= 15);
    REQUIRE(timer.elapsed_ms() < 50);
}

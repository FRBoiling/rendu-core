#include <catch2/catch_test_macros.hpp>
#include <common/io/timer.h>
#include <atomic>
#include <thread>

#include "common/io/io_context.h"

using namespace Rendu::io;

TEST_CASE("Timer one-shot expires_after", "[io][timer]") {
    IoContext io(1);
    Timer timer(io);

    std::atomic<bool> executed{false};
    timer.expires_after(std::chrono::milliseconds(50), [&executed]() { executed = true; });

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE(executed == false);

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE(executed == true);

    io.stop();
}

TEST_CASE("Timer one-shot expires_at", "[io][timer]") {
    IoContext io(1);
    Timer timer(io);

    std::atomic<bool> executed{false};
    auto target_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
    timer.expires_at(target_time, [&executed]() { executed = true; });

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE(executed == false);

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE(executed == true);

    io.stop();
}

TEST_CASE("Timer repeat", "[io][timer]") {
    IoContext io(1);
    Timer timer(io);

    std::atomic<int> count{0};
    timer.repeat(std::chrono::milliseconds(20), [&count]() { count++; });

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(70));
    REQUIRE(count >= 3);

    timer.cancel();
    int count_before_cancel = count.load();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(count == count_before_cancel);

    io.stop();
}

TEST_CASE("Timer cancel before execution", "[io][timer]") {
    IoContext io(1);
    Timer timer(io);

    std::atomic<bool> executed{false};
    timer.expires_after(std::chrono::milliseconds(100), [&executed]() { executed = true; });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.cancel();

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(executed == false);

    io.stop();
}

TEST_CASE("Timer active status", "[io][timer]") {
    IoContext io(1);
    Timer timer(io);

    REQUIRE(timer.active() == false);

    std::atomic<bool> executed{false};
    timer.expires_after(std::chrono::milliseconds(50), [&executed]() { executed = true; });

    REQUIRE(timer.active() == true);

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(70));

    REQUIRE(executed == true);
    REQUIRE(timer.active() == false);

    io.stop();
}

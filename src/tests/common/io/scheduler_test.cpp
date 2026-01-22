#include <catch2/catch_test_macros.hpp>
#include <common/io/scheduler.h>
#include <atomic>
#include <thread>

using namespace Rendu::io;

TEST_CASE("Scheduler post", "[io][scheduler]") {
    IoContext io(1);
    Scheduler scheduler(io);

    std::atomic<int> count{0};
    scheduler.post([&count]() { count++; });
    scheduler.post([&count]() { count++; });
    scheduler.post([&count]() { count++; });

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();

    REQUIRE(count.load() == 3);
}

TEST_CASE("Scheduler delayed", "[io][scheduler]") {
    IoContext io(1);
    Scheduler scheduler(io);

    std::atomic<bool> executed{false};

    std::thread([&io]() { io.run(); }).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    scheduler.delayed([&executed]() { executed = true; }, std::chrono::milliseconds(20));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    REQUIRE(executed.load() == true);

    io.stop();
}

TEST_CASE("Scheduler delayed cancel", "[io][scheduler]") {
    IoContext io(1);
    Scheduler scheduler(io);

    std::atomic<bool> executed{false};
    auto token = scheduler.delayed([&executed]() { executed = true; }, std::chrono::milliseconds(100));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    scheduler.cancel(token);

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(executed.load() == false);

    io.stop();
}

TEST_CASE("Scheduler periodic", "[io][scheduler]") {
    IoContext io(1);
    Scheduler scheduler(io);

    std::atomic<int> count{0};
    auto token = scheduler.periodic([&count]() { count++; }, std::chrono::milliseconds(20));

    std::thread([&io]() { io.run(); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(70));
    REQUIRE(count >= 3);

    scheduler.cancel(token);
    int count_before_cancel = count.load();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(count == count_before_cancel);

    io.stop();
}

//**********************************
//  Created by boil on 2026/01/28.
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <common/io/io_context.h>
#include <thread>
#include <chrono>
#include <atomic>

using namespace Rendu::io;

TEST_CASE("IoContext post", "[io][io_context]") {
    IoContext io(2);
    std::atomic<int> count{0};

    io.post([&count]() { count++; });
    io.post([&count]() { count++; });
    io.post([&count]() { count++; });

    std::thread([&io]() {
        io.run();
    }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    io.stop();

    REQUIRE(count.load() == 3);
}

TEST_CASE("IoContext multi-thread", "[io][io_context]") {
    IoContext io(4);
    std::atomic<int> count{0};

    for (int i = 0; i < 100; ++i) {
        io.post([&count]() { count++; });
    }

    std::thread([&io]() {
        io.run();
    }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    io.stop();

    REQUIRE(count.load() == 100);
}

TEST_CASE("IoContext running status", "[io][io_context]") {
    IoContext io(1);

    REQUIRE(io.running() == false);

    io.run();

    REQUIRE(io.running() == true);

    std::thread([&io]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        io.stop();
    }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    REQUIRE(io.running() == false);
}

TEST_CASE("IoContext stop_after", "[io][io_context]") {
    IoContext io(1);
    std::atomic<int> count{0};

    for (int i = 0; i < 10; ++i) {
        io.post([&count]() { count++; });
    }

    io.run();

    // 使用单独的线程延迟停止
    std::thread([&io]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        io.stop();
    }).detach();

    // 等待所有任务完成和停止完成
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // 所有任务应该被执行
    REQUIRE(count.load() == 10);
    REQUIRE(io.running() == false);
}

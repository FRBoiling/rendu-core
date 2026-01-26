#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <core/loop/fixed_loop.h>
#include <core/loop/loop.h>
#include <common/io/io_context.h>
#include <atomic>
#include <thread>
#include <chrono>

using namespace rendu::core;
using namespace rendu::io;
using namespace std::chrono_literals;

TEST_CASE("FixedLoop basic lifecycle", "[core][loop][basic]") {
    IoContext io(1);
    std::thread io_thread([&io]() { io.run(); });
    io_thread.detach();

    FixedLoop loop(0.016f, io); // ~60 FPS

    SECTION("initial state") {
        REQUIRE(!loop.is_running());
        REQUIRE(!loop.is_paused());
        REQUIRE(loop.total_frames() == 0);
        REQUIRE(loop.target_fps() == 62);
    }

    SECTION("start and stop") {
        loop.start();
        std::this_thread::sleep_for(50ms);
        REQUIRE(loop.is_running());

        loop.stop();
        REQUIRE(!loop.is_running());
        REQUIRE(!loop.is_paused());
    }

    io.stop();
}

TEST_CASE("FixedLoop update callback", "[core][loop][callback]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    FixedLoop loop(0.016f, io);
    std::atomic<int> update_count{0};
    std::atomic<float> last_dt{0.0f};

    loop.set_update_callback([&update_count, &last_dt](float dt) {
        update_count++;
        last_dt.store(dt, std::memory_order_release);

        if (update_count >= 10) {
            // 由外部停止
        }
    });

    loop.start();
    std::this_thread::sleep_for(200ms);
    loop.stop();

    REQUIRE(update_count.load() >= 10);
    REQUIRE(last_dt.load() == Catch::Approx(0.016f).margin(0.001f));

    io.stop();
}

TEST_CASE("FixedLoop pause and resume", "[core][loop][pause]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    FixedLoop loop(0.016f, io);
    std::atomic<int> update_count{0};
    std::atomic<bool> paused_flag{false};

    loop.set_update_callback([&](float dt) {
        if (!paused_flag.load()) {
            update_count++;
        }

        if (update_count >= 5 && !paused_flag.load()) {
            loop.pause();
            paused_flag.store(true);
        }
    });

    loop.start();
    std::this_thread::sleep_for(200ms);

    // 验证暂停状态
    REQUIRE(loop.is_paused());

    // 等待一段时间，确保没有继续更新
    auto count_while_paused = update_count.load();
    std::this_thread::sleep_for(100ms);
    REQUIRE(update_count.load() == count_while_paused);

    // 恢复循环
    loop.resume();
    std::this_thread::sleep_for(50ms);
    REQUIRE(!loop.is_paused());

    loop.stop();
    io.stop();
}

TEST_CASE("FixedLoop target FPS", "[core][loop][fps]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    SECTION("default FPS") {
        FixedLoop loop(0.016f, io);
        REQUIRE(loop.target_fps() == 62);
    }

    SECTION("set target FPS") {
        FixedLoop loop(0.016f, io);
        loop.set_target_fps(120);
        REQUIRE(loop.target_fps() == 120);
        REQUIRE(loop.fixed_delta_time() == Catch::Approx(1.0f / 120.0f).margin(0.0001f));
    }

    SECTION("FPS measurement") {
        FixedLoop loop(0.016f, io);
        std::atomic<bool> running{true};

        loop.set_update_callback([&](float dt) {
            static int count = 0;
            count++;
            if (count >= 100) {
                running.store(false);
            }
        });

        loop.start();
        std::this_thread::sleep_for(200ms);

        float fps = loop.current_fps();
        REQUIRE(fps > 0.0f);

        loop.stop();
        io.stop();
    }
}

TEST_CASE("FixedLoop total frames", "[core][loop][frames]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    FixedLoop loop(0.016f, io);
    std::atomic<int> target_count{50};

    loop.set_update_callback([&](float dt) {
        static int count = 0;
        count++;
        if (count >= target_count.load()) {
            loop.stop();
        }
    });

    loop.start();
    std::this_thread::sleep_for(1200ms);
    loop.stop();

    uint64_t total_frames = loop.total_frames();
    REQUIRE(total_frames >= static_cast<uint64_t>(target_count.load()));

    io.stop();
}

TEST_CASE("FixedLoop stop condition", "[core][loop][stop]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    FixedLoop loop(0.016f, io);
    std::atomic<int> count{0};

    loop.set_update_callback([&](float dt) {
        count++;
        if (count >= 20) {
            loop.stop();
        }
    });

    loop.start();
    std::this_thread::sleep_for(500ms);

    REQUIRE(!loop.is_running());
    REQUIRE(count.load() >= 20);

    io.stop();
}

TEST_CASE("FixedLoop concurrent pause/resume", "[core][loop][concurrent]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    FixedLoop loop(0.016f, io);
    std::atomic<int> update_count{0};
    std::atomic<int> pause_count{0};
    std::atomic<int> resume_count{0};

    loop.set_update_callback([&](float dt) {
        update_count++;

        if (update_count >= 5 && pause_count.load() == 0) {
            loop.pause();
            pause_count.store(1);
        }
    });

    loop.start();
    std::this_thread::sleep_for(100ms);

    // 多次暂停/恢复
    for (int i = 0; i < 3; ++i) {
        if (loop.is_paused()) {
            loop.resume();
            resume_count++;
            std::this_thread::sleep_for(50ms);
        } else {
            loop.pause();
            pause_count++;
            std::this_thread::sleep_for(50ms);
        }
    }

    loop.stop();
    io.stop();

    REQUIRE(resume_count >= 1);
    REQUIRE(pause_count >= 1);
}

TEST_CASE("FixedLoop different delta times", "[core][loop][delta]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    SECTION("30 FPS") {
        FixedLoop loop(1.0f / 30.0f, io);
        REQUIRE(loop.target_fps() == 30);
        REQUIRE(loop.fixed_delta_time() == Catch::Approx(0.033333f).margin(0.0001f));
    }

    SECTION("60 FPS") {
        FixedLoop loop(1.0f / 60.0f, io);
        REQUIRE(loop.target_fps() == 60);
        REQUIRE(loop.fixed_delta_time() == Catch::Approx(0.016667f).margin(0.0001f));
    }

    SECTION("120 FPS") {
        FixedLoop loop(1.0f / 120.0f, io);
        REQUIRE(loop.target_fps() == 120);
        REQUIRE(loop.fixed_delta_time() == Catch::Approx(0.008333f).margin(0.0001f));
    }

    io.stop();
}

TEST_CASE("FixedLoop invalid target FPS", "[core][loop][invalid]") {
    IoContext io(1);
    std::thread([&io]() { io.run(); }).detach();

    FixedLoop loop(0.016f, io);

    // 不应该设置无效的 FPS
    int original_fps = loop.target_fps();
    loop.set_target_fps(0);
    REQUIRE(loop.target_fps() == original_fps);

    loop.set_target_fps(-10);
    REQUIRE(loop.target_fps() == original_fps);

    io.stop();
}

TEST_CASE("FixedLoop stress test", "[core][loop][stress]") {
    IoContext io(2);
    std::thread([&io]() { io.run(); }).detach();

    FixedLoop loop(0.016f, io);
    std::atomic<int> update_count{0};

    loop.set_update_callback([&](float dt) {
        update_count++;
        if (update_count >= 200) {
            loop.stop();
        }
    });

    loop.start();
    std::this_thread::sleep_for(3500ms);

    REQUIRE(update_count.load() >= 200);
    REQUIRE(loop.total_frames() >= 200);

    io.stop();
}

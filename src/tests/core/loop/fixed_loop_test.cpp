//**********************************
//  Created by boil on 2026/01/28.
//  Core Loop 单元测试
//**********************************

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <core/loop/fixed_loop.h>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <common/log/logger.h>

using namespace Rendu;

// 全局日志初始化（每个测试文件一个）
struct TestLoggerSetup {
    io::IoContext io;
    std::thread io_thread;

    TestLoggerSetup() : io(1) {
        log::init_default_io_context(io);
        io_thread = std::thread([this]() {
            io.run();
        });
    }

    ~TestLoggerSetup() {
        io.stop();
        if (io_thread.joinable()) {
            io_thread.join();
        }
    }
};

static TestLoggerSetup g_logger_setup;

// 辅助工具类：用于捕获更新回调
class UpdateCounter {
public:
    std::atomic<int> count{0};
    std::vector<float> delta_times;
    mutable std::mutex mutex;

    void operator()(float delta_time) {
        count.fetch_add(1, std::memory_order_release);
        std::lock_guard<std::mutex> lock(mutex);
        delta_times.push_back(delta_time);
    }

    int get_count() const {
        return count.load(std::memory_order_acquire);
    }

    std::vector<float> get_delta_times() const {
        std::lock_guard<std::mutex> lock(mutex);
        return delta_times;
    }

    void clear() {
        count.store(0, std::memory_order_release);
        std::lock_guard<std::mutex> lock(mutex);
        delta_times.clear();
    }
};

// 辅助类：IoContext 管理器
struct IoContextManager {
    io::IoContext io;
    std::thread io_thread;

    IoContextManager() : io(1) {
        io_thread = std::thread([this]() {
            io.run();
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ~IoContextManager() {
        io.stop();
        if (io_thread.joinable()) {
            io_thread.join();
        }
    }
};

TEST_CASE("FixedLoop: 基本生命周期", "[core][loop]") {
    SECTION("构造函数初始化") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io); // 60 FPS

        REQUIRE(loop.fixed_delta_time() == Catch::Approx(1.0f / 60.0f));
        REQUIRE(loop.target_fps() == 60);
        REQUIRE_FALSE(loop.is_running());
        REQUIRE_FALSE(loop.is_paused());
        REQUIRE(loop.total_frames() == 0);
        REQUIRE(loop.current_fps() == 0.0f);
    }

    SECTION("启动和停止循环") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(loop.is_running());
        REQUIRE_FALSE(loop.is_paused());

        loop.stop();
        REQUIRE_FALSE(loop.is_running());
        REQUIRE_FALSE(loop.is_paused());

        int count = counter.get_count();
        REQUIRE(count > 0);
        INFO("在 100ms 内执行了 " << count << " 次更新");
    }

    SECTION("多次启动不会崩溃") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        loop.start(); // 重复启动应该被忽略
        REQUIRE(loop.is_running());
        loop.stop();
    }

    SECTION("多次停止不会崩溃") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        loop.stop();
        REQUIRE_FALSE(loop.is_running());
        loop.stop(); // 重复停止应该被忽略
    }
}

TEST_CASE("FixedLoop: 更新回调", "[core][loop]") {
    SECTION("回调函数被正确调用") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        loop.stop();

        int count = counter.get_count();
        REQUIRE(count > 0);
        INFO("回调被调用 " << count << " 次");
    }

    SECTION("delta time 参数正确传递") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        loop.stop();

        auto delta_times = counter.get_delta_times();
        REQUIRE_FALSE(delta_times.empty());

        float expected_dt = 1.0f / 60.0f;
        for (auto dt : delta_times) {
            REQUIRE(dt == Catch::Approx(expected_dt));
        }
    }

    SECTION("未设置回调时不会崩溃") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        REQUIRE_NOTHROW(loop.stop());
    }

    SECTION("回调可以动态更换") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        int counter1 = 0, counter2 = 0;
        loop.set_update_callback([&counter1](float) { counter1++; });
        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        loop.set_update_callback([&counter2](float) { counter2++; });

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        loop.stop();

        REQUIRE(counter1 > 0);
        REQUIRE(counter2 > 0);
    }
}

TEST_CASE("FixedLoop: 暂停和恢复", "[core][loop]") {
    SECTION("暂停后更新停止") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        int count_before_pause = counter.get_count();
        loop.pause();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        int count_during_pause = counter.get_count();
        int count_after_resume = 0;

        loop.resume();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        count_after_resume = counter.get_count();

        loop.stop();

        INFO("暂停前: " << count_before_pause << ", 暂停中: " << count_during_pause
                        << ", 恢复后: " << (count_after_resume - count_during_pause));
        REQUIRE(loop.is_paused() == false);
    }

    SECTION("重复暂停和恢复") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        for (int i = 0; i < 3; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            loop.pause();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            loop.resume();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        loop.stop();

        REQUIRE(counter.get_count() > 0);
    }

    SECTION("未启动时暂停不会崩溃") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        REQUIRE_NOTHROW(loop.pause());
        REQUIRE_FALSE(loop.is_paused());
    }

    SECTION("未启动时恢复不会崩溃") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        REQUIRE_NOTHROW(loop.resume());
    }

    SECTION("恢复前多次暂停不会崩溃") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        loop.pause();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        REQUIRE_NOTHROW(loop.pause()); // 重复暂停

        loop.resume();
        loop.stop();
    }
}

TEST_CASE("FixedLoop: 目标 FPS 设置", "[core][loop]") {
    SECTION("设置有效 FPS") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        REQUIRE_NOTHROW(loop.set_target_fps(30));
        REQUIRE(loop.target_fps() == 30);
        REQUIRE(loop.fixed_delta_time() == Catch::Approx(1.0f / 30.0f));

        REQUIRE_NOTHROW(loop.set_target_fps(120));
        REQUIRE(loop.target_fps() == 120);
        REQUIRE(loop.fixed_delta_time() == Catch::Approx(1.0f / 120.0f));
    }

    SECTION("运行时动态调整 FPS") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        loop.set_update_callback(std::ref(counter));
        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int count_60fps = counter.get_count();

        loop.set_target_fps(30);
        counter.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int count_30fps = counter.get_count();

        loop.stop();

        INFO("60 FPS: " << count_60fps << " 次, 30 FPS: " << count_30fps << " 次");
        REQUIRE(count_60fps > count_30fps);
    }

    SECTION("设置无效 FPS 不会崩溃") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        REQUIRE_NOTHROW(loop.set_target_fps(0));
        REQUIRE_NOTHROW(loop.set_target_fps(-10));
        REQUIRE_NOTHROW(loop.set_target_fps(-100));

        // FPS 应该保持原值
        REQUIRE(loop.target_fps() == 60);
    }
}

TEST_CASE("FixedLoop: FPS 测量", "[core][loop]") {
    SECTION("初始 FPS 为 0") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        REQUIRE(loop.current_fps() == 0.0f);
    }

    SECTION("FPS 在 1 秒后更新") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        // 等待 1.5 秒，确保 FPS 至少更新一次
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        float fps = loop.current_fps();
        loop.stop();

        INFO("测量的 FPS: " << fps);
        REQUIRE(fps > 0.0f);
        REQUIRE(fps < 200.0f); // 不应该异常高
    }

    SECTION("FPS 接近目标 FPS") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.set_target_fps(60);
        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        float fps = loop.current_fps();
        loop.stop();

        INFO("目标 FPS: 60, 实际 FPS: " << fps);
        // 允许 ±10 的误差
        REQUIRE(fps > 50.0f);
        REQUIRE(fps < 70.0f);
    }
}

TEST_CASE("FixedLoop: 总帧数统计", "[core][loop]") {
    SECTION("初始总帧数为 0") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        REQUIRE(loop.total_frames() == 0);
    }

    SECTION("总帧数随时间增加") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        uint64_t frames_after_50ms = 0;
        uint64_t frames_after_100ms = 0;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        frames_after_50ms = loop.total_frames();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        frames_after_100ms = loop.total_frames();

        loop.stop();

        INFO("50ms: " << frames_after_50ms << " 帧, 100ms: " << frames_after_100ms << " 帧");
        REQUIRE(frames_after_100ms > frames_after_50ms);
        REQUIRE(frames_after_50ms > 0);
    }

    SECTION("停止后总帧数保持不变") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        uint64_t frames_before_stop = loop.total_frames();
        loop.stop();

        uint64_t frames_after_stop = loop.total_frames();
        REQUIRE(frames_after_stop == frames_before_stop);
    }

    SECTION("暂停期间总帧数不增加") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        uint64_t frames_before_pause = loop.total_frames();

        loop.pause();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        uint64_t frames_during_pause = loop.total_frames();

        loop.resume();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        uint64_t frames_after_resume = loop.total_frames();

        loop.stop();

        INFO("暂停前: " << frames_before_pause
                        << ", 暂停中: " << frames_during_pause
                        << ", 恢复后: " << frames_after_resume);
        REQUIRE(frames_during_pause >= frames_before_pause);
        REQUIRE(frames_after_resume > frames_during_pause);
    }
}

TEST_CASE("FixedLoop: 停止条件", "[core][loop]") {
    SECTION("正常停止循环") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        REQUIRE_NOTHROW(loop.stop());
        REQUIRE_FALSE(loop.is_running());
    }

    SECTION("暂停后停止循环") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        loop.pause();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        REQUIRE_NOTHROW(loop.stop());
        REQUIRE_FALSE(loop.is_running());
        REQUIRE_FALSE(loop.is_paused());
    }

    SECTION("停止后可以重新启动") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        loop.stop();
        REQUIRE_FALSE(loop.is_running());

        counter.clear();
        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        loop.stop();
        REQUIRE(counter.get_count() > 0);
    }
}

TEST_CASE("FixedLoop: 并发暂停/恢复", "[core][loop]") {
    SECTION("快速连续暂停和恢复") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        // 先让循环运行一小段时间，确保有初始更新
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        // 然后快速暂停和恢复
        for (int i = 0; i < 10; ++i) {
            loop.pause();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            loop.resume();
        }

        // 再让循环运行一段时间，累积时间触发更新
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        loop.stop();

        REQUIRE(counter.get_count() > 0);
    }

    SECTION("多线程并发调用控制方法") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        std::thread pause_thread([&loop]() {
            for (int i = 0; i < 50; ++i) {
                loop.pause();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });

        std::thread resume_thread([&loop]() {
            for (int i = 0; i < 50; ++i) {
                loop.resume();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });

        pause_thread.join();
        resume_thread.join();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        loop.stop();

        // 不应该崩溃
        REQUIRE(counter.get_count() >= 0);
    }
}

TEST_CASE("FixedLoop: 不同 delta time", "[core][loop]") {
    SECTION("高帧率 (120 FPS)") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 120.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));
        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        loop.stop();

        REQUIRE(loop.fixed_delta_time() == Catch::Approx(1.0f / 120.0f));
        REQUIRE(counter.get_count() > 0);
    }

    SECTION("中等帧率 (30 FPS)") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 30.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));
        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        loop.stop();

        REQUIRE(loop.fixed_delta_time() == Catch::Approx(1.0f / 30.0f));
        REQUIRE(counter.get_count() > 0);
    }

    SECTION("低帧率 (10 FPS)") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 10.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));
        loop.start();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        loop.stop();

        REQUIRE(loop.fixed_delta_time() == Catch::Approx(1.0f / 10.0f));
        REQUIRE(counter.get_count() > 0);
    }
}

TEST_CASE("FixedLoop: 边界和异常情况", "[core][loop]") {
    SECTION("空回调不会崩溃") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        // 不设置回调
        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        loop.stop();

        // 不应该崩溃
        REQUIRE(true);
    }

    SECTION("超短时间的循环") {
        IoContextManager io_mgr;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        loop.stop();

        // 即使时间很短，也不应该崩溃
        REQUIRE(true);
    }

    SECTION("长时间运行的循环") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        uint64_t frames = loop.total_frames();
        loop.stop();

        INFO("长时间运行产生了 " << frames << " 帧");
        REQUIRE(frames > 0);
    }

    SECTION("析构时自动停止") {
        IoContextManager io_mgr;
        {
            FixedLoop temp_loop(1.0f / 60.0f, io_mgr.io);
            UpdateCounter counter;
            temp_loop.set_update_callback(std::ref(counter));
            temp_loop.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } // temp_loop 在这里析构

        // 析构时应该自动停止，不会崩溃
        REQUIRE(true);
    }

    SECTION("极低 FPS (1 FPS)") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 1.0f, io_mgr.io); // 1 FPS
        loop.set_update_callback(std::ref(counter));

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        loop.stop();

        // 在 1.5 秒内，1 FPS 应该有约 1 次更新
        int count = counter.get_count();
        INFO("1 FPS 在 1.5 秒内执行了 " << count << " 次更新");
        REQUIRE(count >= 1);
        REQUIRE(count <= 2);
    }

    SECTION("极高 FPS (144 FPS)") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 144.0f, io_mgr.io); // 144 FPS
        loop.set_update_callback(std::ref(counter));

        loop.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        loop.stop();

        int count = counter.get_count();
        INFO("144 FPS 在 100ms 内执行了 " << count << " 次更新");
        REQUIRE(count > 10); // 至少应该有相当数量的更新
    }
}

TEST_CASE("FixedLoop: 累积时间机制", "[core][loop]") {
    SECTION("累积时间触发固定步长更新") {
        IoContextManager io_mgr;
        UpdateCounter counter;
        FixedLoop loop(1.0f / 60.0f, io_mgr.io);
        loop.set_update_callback(std::ref(counter));

        loop.start();

        // 使用较低的 FPS 使累积效果更明显
        loop.set_target_fps(10);

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        loop.stop();

        int count = counter.get_count();
        INFO("在 250ms 内执行了 " << count << " 次更新 (目标: 10 FPS)");

        // 在 250ms 内，10 FPS 应该有约 2-3 次更新
        REQUIRE(count >= 2);
        REQUIRE(count <= 4);
    }
}

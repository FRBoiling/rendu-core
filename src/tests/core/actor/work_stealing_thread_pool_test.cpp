#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include "core/actor/work_stealing_thread_pool.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include "common/log/console_sink.h"
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>

using namespace Rendu;

// 全局 IO 对象
static Rendu::io::IoContext g_io(2);

// 初始化日志
struct LogInitializer {
    LogInitializer() {
        Rendu::log::init_default_io_context(g_io);
    }
};
static LogInitializer g_log_initializer;

// ============================================================================
// 基本功能测试
// ============================================================================

TEST_CASE("WorkStealingThreadPool - 基本构造", "[work_stealing_pool]") {
    SECTION("默认线程数构造") {
        WorkStealingThreadPool pool;
        REQUIRE(pool.thread_count() > 0);
    }

    SECTION("指定线程数构造") {
        WorkStealingThreadPool pool(4);
        REQUIRE(pool.thread_count() == 4);
    }
}

TEST_CASE("WorkStealingThreadPool - 基本任务提交", "[work_stealing_pool]") {
    WorkStealingThreadPool pool(2);
    std::atomic<int> counter{0};

    SECTION("单任务执行") {
        pool.submit([&counter]() {
            counter++;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(counter == 1);
    }

    SECTION("多任务执行") {
        const int task_count = 10;

        for (int i = 0; i < task_count; ++i) {
            pool.submit([&counter]() {
                counter++;
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        REQUIRE(counter == task_count);
    }
}

TEST_CASE("WorkStealingThreadPool - 提交到指定线程", "[work_stealing_pool]") {
    WorkStealingThreadPool pool(4);
    std::atomic<int> counters[4] = {0, 0, 0, 0};

    // 分别向不同线程提交任务
    for (size_t i = 0; i < 4; ++i) {
        pool.submit_to_thread(i, [&counters, i]() {
            counters[i]++;
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    for (size_t i = 0; i < 4; ++i) {
        REQUIRE(counters[i] == 1);
    }
}

// ============================================================================
// 工作窃取测试
// ============================================================================

TEST_CASE("WorkStealingThreadPool - 工作窃取", "[work_stealing_pool]") {
    WorkStealingThreadPool pool(2);
    std::atomic<int> counter{0};

    SECTION("负载均衡") {
        const int task_count = 20;

        // 将所有任务提交到线程 0
        for (int i = 0; i < task_count; ++i) {
            pool.submit_to_thread(0, [&counter]() {
                counter++;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // 验证任务被执行
        REQUIRE(counter == task_count);

        // 验证有任务被窃取
        auto stats = pool.get_stats();
        REQUIRE(stats.stolen_count > 0);
    }
}

// ============================================================================
// 多线程并发测试
// ============================================================================

TEST_CASE("WorkStealingThreadPool - 多线程并发提交", "[work_stealing_pool]") {
    WorkStealingThreadPool pool(4);
    std::atomic<int> counter{0};

    SECTION("多线程提交任务") {
        const int thread_count = 4;
        const int tasks_per_thread = 100;
        std::vector<std::thread> threads;

        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&pool, &counter, tasks_per_thread]() {
                for (int j = 0; j < tasks_per_thread; ++j) {
                    pool.submit([&counter]() {
                        counter++;
                    });
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        REQUIRE(counter == thread_count * tasks_per_thread);
    }
}

// ============================================================================
// 统计信息测试
// ============================================================================

TEST_CASE("WorkStealingThreadPool - 统计信息", "[work_stealing_pool]") {
    WorkStealingThreadPool pool(2);
    std::atomic<int> counter{0};

    SECTION("初始统计") {
        auto stats = pool.get_stats();
        REQUIRE(stats.submitted_count == 0);
        REQUIRE(stats.executed_count == 0);
        REQUIRE(stats.stolen_count == 0);
    }

    SECTION("执行后统计") {
        const int task_count = 10;

        for (int i = 0; i < task_count; ++i) {
            pool.submit([&counter]() {
                counter++;
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        auto stats = pool.get_stats();
        REQUIRE(stats.submitted_count >= task_count);
        REQUIRE(stats.executed_count >= task_count);
    }

    SECTION("工作窃取统计") {
        // 将任务全部提交到线程 0
        for (int i = 0; i < 20; ++i) {
            pool.submit_to_thread(0, [&counter]() {
                counter++;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        auto stats = pool.get_stats();
        REQUIRE(stats.stolen_count > 0);
    }
}

// ============================================================================
// 停止测试
// ============================================================================

TEST_CASE("WorkStealingThreadPool - 停止", "[work_stealing_pool]") {
    SECTION("停止后不再执行新任务") {
        WorkStealingThreadPool pool(2);
        std::atomic<int> counter{0};

        pool.stop();

        // 提交任务（不应该被执行）
        pool.submit([&counter]() {
            counter++;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(counter == 0);
    }

    SECTION("停止前执行已提交的任务") {
        WorkStealingThreadPool pool(2);
        std::atomic<int> counter{0};

        const int task_count = 10;

        for (int i = 0; i < task_count; ++i) {
            pool.submit([&counter]() {
                counter++;
            });
        }

        // 等待任务执行完成
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        pool.stop();

        REQUIRE(counter == task_count);
    }
}

// ============================================================================
// 边界情况测试
// ============================================================================

TEST_CASE("WorkStealingThreadPool - 边界情况", "[work_stealing_pool]") {
    SECTION("提交空任务") {
        WorkStealingThreadPool pool(2);
        REQUIRE_NOTHROW(pool.submit(nullptr));
    }

    SECTION("提交到无效线程") {
        WorkStealingThreadPool pool(2);
        REQUIRE_NOTHROW(pool.submit_to_thread(10, []() {}));
    }

    SECTION("单线程池") {
        WorkStealingThreadPool pool(1);
        std::atomic<int> counter{0};

        for (int i = 0; i < 10; ++i) {
            pool.submit([&counter]() {
                counter++;
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        REQUIRE(counter == 10);
    }

    SECTION("大数量任务") {
        WorkStealingThreadPool pool(4);
        std::atomic<int> counter{0};

        const int task_count = 1000;

        for (int i = 0; i < task_count; ++i) {
            pool.submit([&counter]() {
                counter++;
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        REQUIRE(counter == task_count);
    }
}

// ============================================================================
// 异常处理测试
// ============================================================================

TEST_CASE("WorkStealingThreadPool - 异常处理", "[work_stealing_pool]") {
    WorkStealingThreadPool pool(2);
    std::atomic<int> counter{0};
    std::atomic<int> error_count{0};

    SECTION("任务抛出异常") {
        // 部分任务会抛出异常
        for (int i = 0; i < 10; ++i) {
            pool.submit([&counter, &error_count, i]() {
                if (i % 3 == 0) {
                    error_count++;
                    throw std::runtime_error("Test exception");
                }
                counter++;
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // 验证异常不影响其他任务
        REQUIRE(counter + error_count == 10);
    }
}

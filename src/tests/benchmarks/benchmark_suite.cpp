// Rendu Core Performance Benchmark Suite
// 阶段 17: 性能优化基准测试
// 使用 Catch2 的 BENCHMARK 功能 (BENCHMARK("name") { ... } 中可使用 benchmarkIndex 作为迭代次数)

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include "core/actor/message_pool.h"
#include "core/actor/message.h"
#include "common/ser/protobuf_ser.h"
#include "common/net/buffer_view.h"
#include "common/log/async_buffered_logger.h"
#include "core/actor/work_stealing_thread_pool.h"
#include "common/io/io_context.h"
#include "common/log/console_sink.h"
#include "common/log/logger.h"

#include <string>
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

using namespace Rendu;
using namespace Rendu::ser;
using namespace Rendu::net;
using namespace Rendu::log;
using namespace Rendu::io;

// 用于 MessagePool 基准测试的具体消息类型（Message 为抽象类）
struct BenchmarkMsg : public Rendu::Message {
    const char* get_type() const override { return "BenchmarkMsg"; }
    std::string serialize() const override { return ""; }
    void deserialize_data(const std::string&) override {}
};

// ============================================================================
// protobuf 序列化基准测试
// ============================================================================

// 定义测试消息类型
class BenchmarkMessage {
public:
    std::string data;

    // 模拟 protobuf 的 ByteSizeLong
    size_t ByteSizeLong() const {
        return data.size();
    }

    // 模拟 protobuf 的 SerializeToString
    bool SerializeToString(std::string* output) const {
        output->clear();
        output->resize(data.size());
        std::copy(data.begin(), data.end(), output->begin());
        return true;
    }

    // 模拟 protobuf 的 ParseFromArray
    bool ParseFromArray(const void* data_ptr, size_t size) {
        data.clear();
        data.resize(size);
        std::copy(static_cast<const char*>(data_ptr),
                  static_cast<const char*>(data_ptr) + size,
                  data.begin());
        return true;
    }
};

// Catch2 要求 BENCHMARK 必须在 TEST_CASE 内使用（宏展开为 if 语句）
TEST_CASE("Performance benchmarks", "[benchmark]") {
    // 初始化 IoContext（部分基准测试需要）
    IoContext io_ctx(2);
    init_default_io_context(io_ctx);

BENCHMARK("protobuf_serialize_64B", benchmarkIndex) {
    BenchmarkMessage msg;
    msg.data = std::string(64, 'x');

    for (int i = 0; i < benchmarkIndex; ++i) {
        std::string output;
        msg.SerializeToString(&output);
    }
};

BENCHMARK("protobuf_serialize_with_reserve_64B", benchmarkIndex) {
    BenchmarkMessage msg;
    msg.data = std::string(64, 'x');

    for (int i = 0; i < benchmarkIndex; ++i) {
        std::string output;
        output.reserve(msg.ByteSizeLong());
        msg.SerializeToString(&output);
    }
};

BENCHMARK("protobuf_serialize_1KB", benchmarkIndex) {
    BenchmarkMessage msg;
    msg.data = std::string(1024, 'x');

    for (int i = 0; i < benchmarkIndex; ++i) {
        std::string output;
        output.reserve(msg.ByteSizeLong());
        msg.SerializeToString(&output);
    }
};

BENCHMARK("protobuf_serialize_4KB", benchmarkIndex) {
    BenchmarkMessage msg;
    msg.data = std::string(4096, 'x');

    for (int i = 0; i < benchmarkIndex; ++i) {
        std::string output;
        output.reserve(msg.ByteSizeLong());
        msg.SerializeToString(&output);
    }
};

// ============================================================================
// BufferView 零拷贝基准测试
// ============================================================================

BENCHMARK("buffer_view_create_64B", benchmarkIndex) {
    std::string data(64, 'x');

    for (int i = 0; i < benchmarkIndex; ++i) {
        BufferView buffer(data.data(), data.size());
    }
};

BENCHMARK("string_copy_64B", benchmarkIndex) {
    std::string data(64, 'x');

    for (int i = 0; i < benchmarkIndex; ++i) {
        std::string copy(data);
    }
};

BENCHMARK("buffer_view_create_1KB", benchmarkIndex) {
    std::string data(1024, 'x');

    for (int i = 0; i < benchmarkIndex; ++i) {
        BufferView buffer(data.data(), data.size());
    }
};

BENCHMARK("string_copy_1KB", benchmarkIndex) {
    std::string data(1024, 'x');

    for (int i = 0; i < benchmarkIndex; ++i) {
        std::string copy(data);
    }
};

// ============================================================================
// MessagePool 基准测试
// ============================================================================

BENCHMARK("message_pool_allocate", benchmarkIndex) {
    auto& pool = MessagePool::instance();
    for (int i = 0; i < benchmarkIndex; ++i) {
        auto msg = pool.allocate<BenchmarkMsg>();
        pool.deallocate(msg);
    }
};

BENCHMARK("new_delete_message", benchmarkIndex) {
    for (int i = 0; i < benchmarkIndex; ++i) {
        auto msg = new BenchmarkMsg();
        delete msg;
    }
};

// ============================================================================
// WorkStealingThreadPool 基准测试
// ============================================================================

BENCHMARK("work_stealing_thread_pool", benchmarkIndex) {
    WorkStealingThreadPool pool(std::thread::hardware_concurrency());
    std::atomic<int> counter{0};
    std::condition_variable cv;
    std::mutex m;
    const int task_count = 1000;

    for (int i = 0; i < benchmarkIndex; ++i) {
        counter.store(0);
        for (int j = 0; j < task_count; ++j) {
            pool.submit([&counter, &cv, task_count]() {
                counter.fetch_add(1);
                if (counter.load(std::memory_order_relaxed) == task_count) {
                    cv.notify_one();
                }
            });
        }
        std::unique_lock<std::mutex> lock(m);
        cv.wait_for(lock, std::chrono::seconds(5), [&counter, task_count]() {
            return counter.load(std::memory_order_relaxed) >= task_count;
        });
    }
};

BENCHMARK("work_stealing_thread_pool_stealing", benchmarkIndex) {
    WorkStealingThreadPool pool(2);
    std::atomic<int> counter{0};
    std::condition_variable cv;
    std::mutex m;
    const int task_count = 100;

    for (int i = 0; i < benchmarkIndex; ++i) {
        counter.store(0);
        for (int j = 0; j < task_count; ++j) {
            pool.submit_to_thread(0, [&counter, &cv, task_count]() {
                counter.fetch_add(1);
                if (counter.load(std::memory_order_relaxed) == task_count) {
                    cv.notify_one();
                }
            });
        }
        std::unique_lock<std::mutex> lock(m);
        cv.wait_for(lock, std::chrono::seconds(5), [&counter, task_count]() {
            return counter.load(std::memory_order_relaxed) >= task_count;
        });
    }
};

// ============================================================================
// 日志记录基准测试
// ============================================================================

BENCHMARK("logging_sync", benchmarkIndex) {
    IoContext io(1);
    init_default_io_context(io);
    for (int i = 0; i < benchmarkIndex; ++i) {
        RENDU_LOG_INFO("Test message");
    }
    reset_default_io_context();
};

BENCHMARK("logging_async", benchmarkIndex) {
    IoContext io(2);
    AsyncLoggerConfig config;
    config.buffer_size = 64 * 1024;
    config.flush_interval = std::chrono::milliseconds(100);

    AsyncBufferedLogger logger("benchmark", io, config);
    logger.add_sink(std::make_shared<ConsoleSink>());
    logger.set_level(Level::Info);

    for (int i = 0; i < benchmarkIndex; ++i) {
        logger.info("Test message");
    }
};

} // TEST_CASE("Performance benchmarks")



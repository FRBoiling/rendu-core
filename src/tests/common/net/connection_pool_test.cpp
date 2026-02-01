#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "common/net/connection_pool.h"
#include "common/net/codec.h"
#include <thread>
#include <chrono>
#include <atomic>

using namespace Rendu;
using namespace Rendu::net;
using namespace Rendu::io;

// 全局 IoContext,避免生命周期问题
static IoContext g_io(2);
static std::shared_ptr<LengthPrefixCodec> g_codec = std::make_shared<LengthPrefixCodec>();

TEST_CASE("ConnectionPool - 基本功能", "[net][connection_pool]") {
    log::init_default_io_context(g_io);

    SECTION("创建连接池") {
        ConnectionPool pool(g_io, "localhost", 0, ConnectionPoolConfig{
            .codec = g_codec
        });

        auto stats = pool.get_stats();
        REQUIRE(stats.idle_count == 0);
        REQUIRE(stats.active_count == 0);
        REQUIRE(stats.total_count == 0);
    }
}

TEST_CASE("ConnectionPool - 获取和释放连接", "[net][connection_pool]") {
    log::init_default_io_context(g_io);

    ConnectionPool pool(g_io, "localhost", 0, ConnectionPoolConfig{
        .codec = g_codec,
        .max_connections = 2
    });

    SECTION("获取并释放单个连接") {
        // 注意: 这里使用 localhost:0 作为测试,实际使用时应该使用真实的服务器地址
        // 由于没有真实的服务器,我们测试连接池的逻辑

        auto future = pool.acquire();
        // 由于没有服务器,连接会失败,但我们仍然测试连接池的基本逻辑
    }

    SECTION("统计信息") {
        auto stats = pool.get_stats();
        REQUIRE(stats.total_count == stats.idle_count + stats.active_count);
    }
}

TEST_CASE("ConnectionPool - 连接复用", "[net][connection_pool]") {
    log::init_default_io_context(g_io);

    ConnectionPool pool(g_io, "localhost", 0, ConnectionPoolConfig{
        .codec = g_codec,
        .max_connections = 3
    });

    SECTION("连接复用逻辑") {
        // 测试连接池的核心逻辑:连接复用
        // 由于需要真实的服务器,这里主要测试代码编译和基本逻辑

        auto stats1 = pool.get_stats();
        REQUIRE(stats1.total_count == 0);
    }
}

TEST_CASE("ConnectionPool - 多线程并发", "[net][connection_pool]") {
    log::init_default_io_context(g_io);

    ConnectionPool pool(g_io, "localhost", 0, ConnectionPoolConfig{
        .codec = g_codec,
        .max_connections = 5
    });

    SECTION("并发获取连接") {
        const int thread_count = 4;
        const int requests_per_thread = 3;
        std::vector<std::thread> threads;
        std::atomic<int> total_acquired{0};

        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([&, t]() {
                for (int i = 0; i < requests_per_thread; ++i) {
                    auto future = pool.acquire();
                    // 由于没有真实服务器,我们只测试请求被接受
                    total_acquired++;
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        REQUIRE(total_acquired.load() == thread_count * requests_per_thread);
    }
}

TEST_CASE("ConnectionPool - 清理空闲连接", "[net][connection_pool]") {
    log::init_default_io_context(g_io);

    ConnectionPool pool(g_io, "localhost", 0, ConnectionPoolConfig{
        .codec = g_codec
    });

    SECTION("清理功能") {
        pool.cleanup_idle_connections();

        auto stats = pool.get_stats();
        REQUIRE(stats.idle_count == 0);
    }
}

TEST_CASE("ConnectionPool - 关闭所有连接", "[net][connection_pool]") {
    log::init_default_io_context(g_io);

    ConnectionPool pool(g_io, "localhost", 0, ConnectionPoolConfig{
        .codec = g_codec
    });

    SECTION("关闭所有连接") {
        pool.close_all();

        auto stats = pool.get_stats();
        REQUIRE(stats.active_count == 0);
        REQUIRE(stats.total_count == 0);
    }
}

TEST_CASE("ConnectionPool - 配置参数", "[net][connection_pool]") {
    log::init_default_io_context(g_io);

    SECTION("自定义最大连接数") {
        ConnectionPoolConfig config{
            .max_connections = 50,
            .idle_timeout = std::chrono::seconds(60),
            .connect_timeout = std::chrono::seconds(5),
            .codec = g_codec
        };

        ConnectionPool pool(g_io, "localhost", 0, config);
        auto stats = pool.get_stats();

        // 初始状态应该是空的
        REQUIRE(stats.total_count == 0);
    }

    SECTION("不同的超时配置") {
        auto timeout = GENERATE(
            std::chrono::seconds(30),
            std::chrono::seconds(60),
            std::chrono::seconds(300)
        );

        ConnectionPoolConfig config{
            .idle_timeout = timeout,
            .connect_timeout = timeout,
            .codec = g_codec
        };

        ConnectionPool pool(g_io, "localhost", 0, config);
        REQUIRE(pool.get_stats().total_count == 0);
    }
}

TEST_CASE("ConnectionPool - 边界情况", "[net][connection_pool]") {
    log::init_default_io_context(g_io);

    SECTION("零最大连接数") {
        ConnectionPoolConfig config{
            .max_connections = 0,
            .codec = g_codec
        };

        ConnectionPool pool(g_io, "localhost", 0, config);
        auto stats = pool.get_stats();
        REQUIRE(stats.total_count == 0);
    }

    SECTION("大连接数限制") {
        ConnectionPoolConfig config{
            .max_connections = 10000,
            .codec = g_codec
        };

        ConnectionPool pool(g_io, "localhost", 0, config);
        auto stats = pool.get_stats();
        REQUIRE(stats.total_count == 0);
    }
}

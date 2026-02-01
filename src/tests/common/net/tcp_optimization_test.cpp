#include "common/net/socket.h"
#include "common/io/io_context.h"
#include "common/net/connection_pool.h"
#include "common/net/codec.h"
#include "common/log/logger.h"
#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <chrono>

// 全局 IO 对象和编码器,避免测试中重复创建导致全局 logger 指针问题
static Rendu::io::IoContext g_io(2);
static std::shared_ptr<Rendu::net::LengthPrefixCodec> g_codec =
    std::make_shared<Rendu::net::LengthPrefixCodec>();

TEST_CASE("TcpOptimization - 默认配置构造", "[tcp_optimization]") {
    Rendu::net::TcpOptimization opts;

    SECTION("默认值符合预期") {
        REQUIRE(opts.no_delay == true);
        REQUIRE(opts.keepalive == true);
        REQUIRE(opts.keepalive_idle == 60);
        REQUIRE(opts.keepalive_interval == 10);
        REQUIRE(opts.keepalive_count == 3);
        REQUIRE(opts.recv_buffer_size == 64 * 1024);
        REQUIRE(opts.send_buffer_size == 64 * 1024);
        REQUIRE(opts.reuse_address == true);
        REQUIRE(opts.reuse_port == false);
        REQUIRE(opts.tcp_no_delay == true);
    }
}

TEST_CASE("TcpSocket - 默认优化参数", "[tcp_optimization]") {
    Rendu::net::TcpSocket socket(g_io);

    SECTION("Socket 成功打开并应用默认优化") {
        REQUIRE(socket.is_open() == true);
    }
}

TEST_CASE("TcpSocket - 自定义优化参数", "[tcp_optimization]") {
    Rendu::net::TcpOptimization opts;
    opts.no_delay = false;
    opts.keepalive = false;
    opts.recv_buffer_size = 128 * 1024;
    opts.send_buffer_size = 128 * 1024;

    Rendu::net::TcpSocket socket(g_io, opts);

    SECTION("Socket 成功打开") {
        REQUIRE(socket.is_open() == true);
    }

    SECTION("禁用 Nagle 算法的 socket") {
        REQUIRE(socket.is_open() == true);
    }
}

TEST_CASE("TcpOptimization - 低延迟配置", "[tcp_optimization]") {
    Rendu::net::TcpOptimization opts;
    opts.no_delay = true;
    opts.keepalive = true;
    opts.keepalive_idle = 30;
    opts.recv_buffer_size = 32 * 1024;
    opts.send_buffer_size = 32 * 1024;

    Rendu::net::TcpSocket socket(g_io, opts);

    SECTION("低延迟配置 socket 成功创建") {
        REQUIRE(socket.is_open() == true);
    }
}

TEST_CASE("TcpOptimization - 高吞吐量配置", "[tcp_optimization]") {
    Rendu::net::TcpOptimization opts;
    opts.no_delay = false;
    opts.keepalive = true;
    opts.recv_buffer_size = 256 * 1024;
    opts.send_buffer_size = 256 * 1024;

    Rendu::net::TcpSocket socket(g_io, opts);

    SECTION("高吞吐量配置 socket 成功创建") {
        REQUIRE(socket.is_open() == true);
    }
}

TEST_CASE("TcpOptimization - 自定义 keepalive 参数", "[tcp_optimization]") {
    Rendu::net::TcpOptimization opts;
    opts.keepalive = true;
    opts.keepalive_idle = 120;
    opts.keepalive_interval = 20;
    opts.keepalive_count = 5;

    Rendu::net::TcpSocket socket(g_io, opts);

    SECTION("自定义 keepalive 参数 socket 成功创建") {
        REQUIRE(socket.is_open() == true);
    }
}

TEST_CASE("TcpSocket - 连接测试", "[tcp_optimization]") {
    Rendu::log::init_default_io_context(g_io);

    // 创建 TCP 服务器
    Rendu::net::TcpAcceptor acceptor(g_io, 0);  // 0 表示自动分配端口
    uint16_t port = acceptor.local_endpoint().port();

    Rendu::net::TcpOptimization opts;
    opts.no_delay = true;

    Rendu::net::TcpSocket client_socket(g_io, opts);

    SECTION("成功连接到服务器") {
        bool connected = false;

        client_socket.async_connect("127.0.0.1", port,
            [&connected](const boost::system::error_code& ec) {
                if (!ec) {
                    connected = true;
                }
            });

        // 运行 IO 上下文来处理异步操作
        g_io.native().run_for(std::chrono::milliseconds(100));

        REQUIRE(connected == true);
    }
}

TEST_CASE("TcpSocket - 多个 socket 使用不同优化", "[tcp_optimization]") {
    Rendu::net::TcpOptimization low_latency_opts;
    low_latency_opts.no_delay = true;
    low_latency_opts.recv_buffer_size = 32 * 1024;

    Rendu::net::TcpOptimization high_throughput_opts;
    high_throughput_opts.no_delay = false;
    high_throughput_opts.recv_buffer_size = 256 * 1024;

    Rendu::net::TcpSocket socket1(g_io, low_latency_opts);
    Rendu::net::TcpSocket socket2(g_io, high_throughput_opts);
    Rendu::net::TcpSocket socket3(g_io);  // 默认配置

    SECTION("多个 socket 成功创建") {
        REQUIRE(socket1.is_open() == true);
        REQUIRE(socket2.is_open() == true);
        REQUIRE(socket3.is_open() == true);
    }
}

TEST_CASE("TcpSocket - reuse_port 选项", "[tcp_optimization]") {
    Rendu::net::TcpOptimization opts;
    opts.reuse_port = true;

    Rendu::net::TcpSocket socket(g_io, opts);

    SECTION("启用 reuse_port 的 socket 成功创建") {
        REQUIRE(socket.is_open() == true);
    }

    SECTION("多个 socket 可以绑定同一端口 (如果平台支持)") {
        // 在支持 SO_REUSEPORT 的平台上,多个 socket 可以绑定同一端口
        // 这里只验证 socket 创建成功,不测试实际绑定
        Rendu::net::TcpSocket socket2(g_io, opts);
        REQUIRE(socket2.is_open() == true);
    }
}

TEST_CASE("TcpSocket - 边界值测试", "[tcp_optimization]") {
    SECTION("最小缓冲区大小") {
        Rendu::net::TcpOptimization opts;
        opts.recv_buffer_size = 1024;
        opts.send_buffer_size = 1024;

        Rendu::net::TcpSocket socket(g_io, opts);
        REQUIRE(socket.is_open() == true);
    }

    SECTION("最大缓冲区大小") {
        Rendu::net::TcpOptimization opts;
        opts.recv_buffer_size = 1024 * 1024;  // 1MB
        opts.send_buffer_size = 1024 * 1024;  // 1MB

        Rendu::net::TcpSocket socket(g_io, opts);
        REQUIRE(socket.is_open() == true);
    }

    SECTION("零 keepalive 参数") {
        Rendu::net::TcpOptimization opts;
        opts.keepalive_idle = 0;
        opts.keepalive_interval = 0;
        opts.keepalive_count = 0;

        Rendu::net::TcpSocket socket(g_io, opts);
        REQUIRE(socket.is_open() == true);
    }

    SECTION("禁用所有优化") {
        Rendu::net::TcpOptimization opts;
        opts.no_delay = false;
        opts.keepalive = false;
        opts.recv_buffer_size = 0;
        opts.send_buffer_size = 0;
        opts.reuse_address = false;
        opts.reuse_port = false;

        Rendu::net::TcpSocket socket(g_io, opts);
        REQUIRE(socket.is_open() == true);
    }
}

TEST_CASE("TcpOptimization - 配置一致性", "[tcp_optimization]") {
    Rendu::net::TcpOptimization opts;

    SECTION("no_delay 和 tcp_no_delay 默认值相同") {
        REQUIRE(opts.no_delay == opts.tcp_no_delay);
    }

    SECTION("设置 no_delay 时 tcp_no_delay 独立") {
        opts.no_delay = true;
        opts.tcp_no_delay = false;

        REQUIRE(opts.no_delay == true);
        REQUIRE(opts.tcp_no_delay == false);
    }
}

TEST_CASE("TcpSocket - 与 ConnectionPool 集成", "[tcp_optimization]") {
    Rendu::log::init_default_io_context(g_io);

    // 注意: ConnectionPool 目前不支持 TcpOptimization 配置
    // 这个测试验证 ConnectionPool 可以正常创建

    Rendu::net::ConnectionPoolConfig config;
    config.max_connections = 10;
    config.codec = g_codec;

    Rendu::net::ConnectionPool pool(g_io, "127.0.0.1", 12345, config);

    SECTION("连接池创建成功") {
        auto stats = pool.get_stats();
        REQUIRE(stats.idle_count == 0);
        REQUIRE(stats.active_count == 0);
        REQUIRE(stats.total_count == 0);
    }
}

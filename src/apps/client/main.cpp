#include "test_client.h"
#include "performance_test.h"
#include "performance_test_config.h"
#include "simple_test.h"
#include "common/log/logger.h"
#include "common/io/io_context.h"
#include <iostream>
#include <string>
#include <thread>

using namespace Rendu;
using namespace Rendu::net;
using namespace Rendu::client;

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <mode> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  test          Run simple test (default)" << std::endl;
    std::cout << "  perf          Run performance test" << std::endl;
    std::cout << std::endl;
    std::cout << "Test mode options:" << std::endl;
    std::cout << "  --host <host>    Server host (default: 127.0.0.1)" << std::endl;
    std::cout << "  --port <port>    Server port (default: 8080)" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance test options:" << std::endl;
    std::cout << "  --clients N       Number of clients (default: 100)" << std::endl;
    std::cout << "  --duration N      Test duration in seconds (default: 60)" << std::endl;
    std::cout << "  --interval N      Chat interval in ms (default: 1000)" << std::endl;
    std::cout << "  --host <host>     Server host (default: 127.0.0.1)" << std::endl;
    std::cout << "  --port <port>     Server port (default: 8080)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << " test" << std::endl;
    std::cout << "  " << program_name << " test --host 192.168.1.100 --port 9000" << std::endl;
    std::cout << "  " << program_name << " perf" << std::endl;
    std::cout << "  " << program_name << " perf --clients 1000 --duration 120" << std::endl;
}

int main(int argc, char* argv[]) {
    // 初始化日志
    io::IoContext log_io;
    log::init_default_io_context(log_io);

    // 创建默认 logger
    auto logger = std::make_shared<log::Logger>("client", log_io);
    logger->set_level(log::Level::Info);
    log::set_default_logger(logger);

    if (argc < 2) {
        print_usage(argv[0]);
        return 0;
    }

    std::string mode = argv[1];

    if (mode == "--help" || mode == "-h") {
        print_usage(argv[0]);
        return 0;
    }

    if (mode == "test") {
        // 简单测试模式
        std::string host = "127.0.0.1";
        uint16_t port = 8080;

        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--host" && i + 1 < argc) {
                host = argv[++i];
            } else if (arg == "--port" && i + 1 < argc) {
                port = static_cast<uint16_t>(std::stoi(argv[++i]));
            } else if (arg == "--help" || arg == "-h") {
                print_usage(argv[0]);
                return 0;
            }
        }

        run_simple_test(host, port);

    } else if (mode == "perf") {
        // 性能测试模式
        TestConfig config;

        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--clients" && i + 1 < argc) {
                config.num_clients = std::stoi(argv[++i]);
            } else if (arg == "--duration" && i + 1 < argc) {
                config.test_duration_sec = std::stoi(argv[++i]);
            } else if (arg == "--interval" && i + 1 < argc) {
                config.chat_interval_ms = std::stoi(argv[++i]);
            } else if (arg == "--host" && i + 1 < argc) {
                config.server_host = argv[++i];
            } else if (arg == "--port" && i + 1 < argc) {
                config.server_port = static_cast<uint16_t>(std::stoi(argv[++i]));
            } else if (arg == "--help" || arg == "-h") {
                print_usage(argv[0]);
                return 0;
            }
        }

        try {
            PerformanceTest test(config);
            test.run();
        } catch (const std::exception& e) {
            std::cerr << "Test failed: " << e.what() << std::endl;
            return 1;
        }

    } else {
        std::cerr << "Unknown mode: " << mode << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}

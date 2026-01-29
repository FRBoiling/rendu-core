#pragma once

#include "test_client.h"
#include "performance_test_config.h"
#include "common/io/io_context.h"
#include "common/log/logger.h"
#include <memory>
#include <vector>

BEGIN_NAMESPACE_COMMON
namespace client {

/**
 * @brief 性能测试类
 */
class PerformanceTest {
public:
    explicit PerformanceTest(const TestConfig& config);

    /**
     * @brief 运行性能测试
     */
    void run();

private:
    /**
     * @brief 连接所有客户端
     */
    void connect_clients();

    /**
     * @brief 断开所有客户端
     */
    void disconnect_clients();

    /**
     * @brief 开始聊天循环
     */
    void start_chat_loop();

    /**
     * @brief 停止聊天循环
     */
    void stop_chat_loop();

    /**
     * @brief 打印测试结果
     */
    void print_results();

    /**
     * @brief 格式化字节数
     */
    static std::string format_bytes(uint64_t bytes);

private:
    TestConfig config_;
    std::unique_ptr<io::IoContext> io_;
    std::vector<std::shared_ptr<net::TestClient>> clients_;
};

} // namespace client
END_NAMESPACE_COMMON

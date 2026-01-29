#pragma once

#include "common/io/io_context.h"
#include <string>

BEGIN_NAMESPACE_COMMON
namespace client {

/**
 * @brief 运行简单测试模式
 * @param host 服务器主机地址
 * @param port 服务器端口
 */
void run_simple_test(const std::string& host, uint16_t port);

} // namespace client
END_NAMESPACE_COMMON

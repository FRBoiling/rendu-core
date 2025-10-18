#include "EcsServer.h"

#include <cstdlib>
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
  std::cout << "Starting ECS Server Framework..." << std::endl;

  try {
    // 创建服务器实例
    g_server = std::make_unique<EcsServer>();

    // 解析命令行参数
    uint16_t port = 8080;
    if (argc > 1) {
      port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    std::cout << "Initializing server on port " << port << "..." << std::endl;

    // 初始化服务器
    if (!g_server->initialize(port)) {
      std::cerr << "Failed to initialize server" << std::endl;
      return 1;
    }

    std::cout << "Server initialized successfully. Starting main loop..." << std::endl;
    std::cout << "Press Ctrl+C to stop the server." << std::endl;

    // 运行服务器
    g_server->run();

    std::cout << "Server stopped normally." << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "Fatal error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
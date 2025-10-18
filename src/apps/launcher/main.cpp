// main.cpp

#include "core/log/LogSystem.hpp"
#include "core/network/NetworkSystem.hpp"

using namespace core;

int main() {
    try {
        World world;
        
        // 初始化日志系统
        world.addSystem<LogSystem>();
        
        // 初始化网络系统
        world.addSystem<NetworkSystem>();

        // 主循环
        auto lastTime = std::chrono::high_resolution_clock::now();
        while (true) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            double deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            world.update(deltaTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } catch (const std::exception& e) {
        // 错误处理
    }
    return 0;
}
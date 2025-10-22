#include "common/ecs/world.h"
#include "common/ecs/system_group.h"
#include "player/logic_system.h"
#include "common/asio/io_context.h"
#include "common/ecs/world_plugin_extension.h"
#include "network/network_system.h"
#include "common/log/logging.h"

int main() {
    // 创建日志器
    common::log::Logger::get_instance().initialize();
    
    // 创建IoContext用于异步操作
    common::asio::IoContext io_context;
    
    // 创建ECS世界
    auto world = std::make_unique<common::Ecs::World>();
    
    // 注册移动插件
    common::Ecs::WorldPluginExtension::register_plugin<MovementPlugin>(*world);
    
    // 创建并添加网络系统
    world->add_system<NetworkSystem>(io_context, 8080);
    
    // 初始化世界
    world->initialize();
    
    // 启动网络线程
    std::thread network_thread([&io_context]() {
        io_context.run();
    });
    
    // 主循环
    const float delta_time = 1.0f / 60.0f;
    for (int i = 0; i < 100; ++i) {
        // 创建玩家实体
        auto entity = world->get_entity_factory().create("Player");
        
        // 添加位置组件
        world->get_registry().emplace<PositionComponent>(entity);
        auto& position = world->get_registry().get<PositionComponent>(entity);
        position.x = 0.0f;
        position.y = 0.0f;
        position.z = 0.0f;
        
        // 添加速度组件
        world->get_registry().emplace<VelocityComponent>(entity);
        auto& velocity = world->get_registry().get<VelocityComponent>(entity);
        velocity.x = 1.0f;
        velocity.y = 0.0f;
        velocity.z = 0.0f;
        
        // 更新世界
        world->update(delta_time);
        
        // 模拟一小段延迟
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    // 关闭网络
    io_context.stop();
    if (network_thread.joinable()) {
        network_thread.join();
    }
    
    // 关闭插件和世界
    world->shutdown();
    
    return 0;
}
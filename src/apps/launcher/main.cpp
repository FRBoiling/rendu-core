#include "common/ecs/application.h"
#include <iostream>

using namespace common::Ecs;
// 示例应用程序
class MyApplication : public Application {
protected:
    void register_systems() override {
        // 这里注册你的系统
        // world_->add_system<MySystem>();
    }
    
    void update(float delta_time) override {
        // 应用程序特定更新逻辑
    }
    
    void render() override {
        // 渲染逻辑
    }
};

int main(int argc, char* argv[]) {
    try {
        MyApplication app;
        
        if (!app.initialize()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return 1;
        }
        
        std::cout << "Starting application: " << app.get_name() << std::endl;
        app.run();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 1;
    }
}
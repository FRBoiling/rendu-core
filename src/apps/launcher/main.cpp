#include <iostream>
#include "common/ecs/application.h"
#include "core/logging/logger_system.h"


using namespace Rendu::Ecs;
using namespace Rendu::Core;

// 示例应用程序
class MyApplication : public Application {
protected:
    void register_systems() override {
         world_->add_system<LoggerSystem>();
    }

    void update(float delta_time) override {
        // 应用程序特定更新逻辑
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

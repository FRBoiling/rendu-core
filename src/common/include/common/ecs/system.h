#pragma once

#include "common/define.h"
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <functional>

BEGIN_NAMESPACE_COMMON
namespace ecs {

class Registry;

class System {
public:
    System() = default;
    virtual ~System() = default;

    virtual void update(float delta_time) = 0;
    virtual std::string name() const = 0;
    virtual int priority() const { return 0; } // 越小越先执行

    void set_enabled(bool enabled) { enabled_ = enabled; }
    bool is_enabled() const { return enabled_; }

protected:
    bool enabled_ = true;
};

class SystemManager {
public:
    SystemManager() = default;
    ~SystemManager() = default;

    void add_system(std::shared_ptr<System> system) {
        if (!system) return;
        
        systems_.push_back(system);
        // 按优先级排序
        std::sort(systems_.begin(), systems_.end(),
            [](const std::shared_ptr<System>& a, const std::shared_ptr<System>& b) {
                return a->priority() < b->priority();
            });
    }

    void remove_system(const std::string& name) {
        systems_.erase(
            std::remove_if(systems_.begin(), systems_.end(),
                [&name](const std::shared_ptr<System>& s) {
                    return s->name() == name;
                }),
            systems_.end());
    }

    void update(float delta_time) {
        for (auto& system : systems_) {
            if (system && system->is_enabled()) {
                system->update(delta_time);
            }
        }
    }

    size_t system_count() const { return systems_.size(); }

    const std::vector<std::shared_ptr<System>>& systems() const {
        return systems_;
    }

private:
    std::vector<std::shared_ptr<System>> systems_;
};

} // namespace ecs
END_NAMESPACE_COMMON

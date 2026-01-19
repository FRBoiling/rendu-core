#include "common/ecs/registry_optimized.h"
#include "common/ecs/profiling/entity_browser.h"
#include <iostream>

struct Position {
    float x, y;
    Position() : x(0), y(0) {}
    Position(float x_, float y_) : x(x_), y(y_) {}
};

struct Velocity {
    float vx, vy;
    Velocity() : vx(0), vy(0) {}
    Velocity(float vx_, float vy_) : vx(vx_), vy(vy_) {}
};

int main() {
    Rendu::RegistryOptimized registry;
    Rendu::EntityBrowser browser;

    auto e1 = registry.create();
    registry.emplace<Position>(e1, Position{1.0f, 2.0f});

    auto e2 = registry.create();
    registry.emplace<Position>(e2, Position{3.0f, 4.0f});
    registry.emplace<Velocity>(e2, Velocity{1.0f, 1.0f});

    std::cout << "Refreshing browser...\n";
    browser.refresh(registry);

    auto entities = browser.getEntities();
    std::cout << "Total entities: " << entities.size() << "\n";

    for (const auto& info : entities) {
        std::cout << "Entity " << info.entity.value() << ":\n";
        std::cout << "  Valid: " << (info.valid ? "yes" : "no") << "\n";
        std::cout << "  Components (" << info.components.size() << "):\n";
        for (const auto& comp : info.components) {
            std::cout << "    - " << comp.typeName << ": " << comp.value << "\n";
        }
    }

    auto positionEntities = browser.searchByComponent("Position");
    std::cout << "\nEntities with Position: " << positionEntities.size() << "\n";

    auto velocityEntities = browser.searchByComponent("Velocity");
    std::cout << "Entities with Velocity: " << velocityEntities.size() << "\n";

    return 0;
}

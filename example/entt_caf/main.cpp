#include <caf/all.hpp>
#include <entt/entt.hpp>
#include <iostream>

struct position {
  double x, y;
};

struct velocity {
  double dx, dy;
};

class physics_system : public caf::event_based_actor {
public:
  physics_system(caf::actor_config& cfg) : caf::event_based_actor(cfg) {}

  caf::behavior make_behavior() override {
    return {
        [=](const std::string& cmd) {
          if (cmd == "update") {
            registry.view<position, velocity>().each([](auto& pos, auto& vel) {
              pos.x += vel.dx;
              pos.y += vel.dy;
              std::cout << "Updated position: (" << pos.x << ", " << pos.y << ")\n";
            });
          } else if (cmd == "create") {
            auto entity = registry.create();
            registry.emplace<position>(entity, 0.0, 0.0);
            registry.emplace<velocity>(entity, 1.0, 1.0);
            std::cout << "Created entity with position (0.0, 0.0) and velocity (1.0, 1.0)\n";
          }
        }
    };
  }

private:
  entt::registry registry;
};

void caf_main(caf::actor_system& system) {
  auto physics = system.spawn<physics_system>();
  caf::scoped_actor self{system};
  self->send(physics, "create");
  self->send(physics, "update");
}

CAF_MAIN()
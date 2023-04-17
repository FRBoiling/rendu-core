#include <caf/all.hpp>
#include <entt/entt.hpp>

namespace actor_example {
    using ping_atom = caf::atom_constant<caf::atom("ping")>;
    using pong_atom = caf::atom_constant<caf::atom("pong")>;

    struct ping_msg {
        caf::actor from;
    };

    struct pong_msg {
        caf::actor from;
    };

    struct position {
        int x;
        int y;
    };

    struct velocity {
        double x;
        double y;
    };

    struct move_actor : public caf::event_based_actor {
        move_actor(caf::actor_config& cfg, entt::registry& registry)
                : caf::event_based_actor(cfg), registry_(registry) {
        }

        ~move_actor() override = default;

        caf::behavior make_behavior() override {
            return {
                    [=](const caf::io::new_data_msg& msg) {
                        // 处理IO消息
                    },
                    [=](const caf::io::new_connection_msg& msg) {
                        // 处理IO消息
                    },
                    [=](ping_atom, ping_msg& msg) -> caf::message {
                        // 用Entity，并更新Component
                        auto entity = registry_.create();
                        registry_.emplace<position>(entity, 0, 0);
                        registry_.emplace<velocity>(entity, 1.0, 1.0);
                        // 发送消息到其它Actor
                        return pong_atom::value, pong_msg{this};
                    },
                    [=](pong_atom, pong_msg& msg) -> caf::message {
                        // 接收其它Actor的消息
                        // 处理Entity并更新Component
                        auto view = registry_.view<position, velocity>();
                        for (auto entity : view) {
                            auto& pos = view.get<position>(entity);
                            auto& vel = view.get<velocity>(entity);
                            pos.x += vel.x;
                            pos.y += vel.y;
                        }
                        // 发送消息到其它Actor
                        return ping_atom::value, ping_msg{this};
                    },
            };
        }

        entt::registry& registry_;
    };
}

void run_example() {
    caf::actor_system_config cfg;
    caf::actor_system system{cfg};

    entt::registry registry;

    auto move_actor_ptr = system.spawn<actor_example::move_actor>(std::ref(registry));

    auto timer_actor = system.spawn<caf::scheduled_actor>();
    caf::anon_send(timer_actor, caf::after(std::chrono::seconds(1)), actor_example::ping_atom::value, actor_example::ping_msg{move_actor_ptr});

    system.await_all_actors_done();
}

int main(){
    run_example();
}

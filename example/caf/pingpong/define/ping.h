/*
* Created by boil on 2023/3/26.
*/

#ifndef EXAMPLE_ECS_DEFINE_PING_H_
#define EXAMPLE_ECS_DEFINE_PING_H_

#include "config.h"

struct ping_state {
  size_t count = 0;
};


caf::behavior ping(caf::stateful_actor<ping_state>* self, size_t num_pings) {
  print_on_exit(self, "ping");
  return {
      [=](kickoff_atom, const caf::actor& pong) {
        self->send(pong, caf::ping_atom_v, 1);
        self->become([=](caf::pong_atom, int value) -> caf::result<caf::ping_atom, int> {
          if (++(self->state.count) >= num_pings)
            self->quit();
          return {caf::ping_atom_v, value + 1};
        });
      },
  };
}


void run_client(caf::actor_system& system, const config& cfg) {
  std::cout << "run in client mode" << std::endl;
  auto ping_actor = system.spawn(ping, 20u);
  auto io_actor = system.middleman().spawn_client(protobuf_io, cfg.host,
                                                  cfg.port, ping_actor);
  if (!io_actor) {
    std::cout << "cannot connect to " << cfg.host << " at GetPort " << cfg.port
              << ": " << to_string(io_actor.error()) << std::endl;
    return;
  }
  send_as(*io_actor, ping_actor, kickoff_atom_v, *io_actor);
}

#endif //EXAMPLE_ECS_DEFINE_PING_H_

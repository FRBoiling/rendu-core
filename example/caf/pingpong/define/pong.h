/*
* Created by boil on 2023/3/26.
*/

#ifndef EXAMPLE_ECS_DEFINE_PONG_H_
#define EXAMPLE_ECS_DEFINE_PONG_H_

#include "config.h"

caf::behavior server(caf::io::broker *self, const caf::actor &buddy) {
  print_on_exit(self, "server");
  aout(self) << "server is running" << std::endl;
  return {
      [=](const caf::io::new_connection_msg &msg) {
        aout(self) << "server accepted new connection" << std::endl;
        auto io_actor = self->fork(protobuf_io, msg.handle, buddy);
        // only accept 1 connection in our example
        self->quit();
      },
  };
}

caf::behavior pong(caf::event_based_actor *self) {
  print_on_exit(self, "pong");
  return {
      [=](caf::ping_atom, int value) { return make_message(caf::pong_atom_v, value); },
  };
}

void run_server(caf::actor_system &system, const config &cfg) {
  std::cout << "run in server mode" << std::endl;
  auto pong_actor = system.spawn(pong);
  auto server_actor = system.middleman().spawn_server(server, cfg.port, pong_actor);
  if (!server_actor)
    std::cerr << "unable to spawn server: " << to_string(server_actor.error()) << std::endl;
}

#endif //EXAMPLE_ECS_DEFINE_PONG_H_

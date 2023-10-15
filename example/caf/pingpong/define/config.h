/*
* Created by boil on 2023/3/26.
*/

#ifndef EXAMPLE_ECS_DEFINE_CONFIG_H_
#define EXAMPLE_ECS_DEFINE_CONFIG_H_
#include "defines.h"
#include <caf/all.hpp>
#include <caf/io/all.hpp>

class config : public caf::actor_system_config {
 public:
  uint16_t port = 0;
  std::string host = "localhost";
  bool server_mode = false;

  config() {
    opt_group{custom_options_, "global"}
        .add(port, "GetPort,p", "set GetPort")
        .add(host, "host,H", "set host (ignored in server mode)")
        .add(server_mode, "server-mode,s", "enable server mode");
  }
};

#endif //EXAMPLE_ECS_DEFINE_CONFIG_H_

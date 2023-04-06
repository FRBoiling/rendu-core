/*
* Created by boil on 2023/3/26.
*/

#ifndef EXAMPLE_ECS_INCLUDE_PROTO_TYPE_ID_H_
#define EXAMPLE_ECS_INCLUDE_PROTO_TYPE_ID_H_

#include <caf/all.hpp>
#include <caf/io/all.hpp>

#ifdef CAF_WINDOWS
#pragma comment(lib, "ws2_32")
#  include <winsock2.h>
#else
#  include <arpa/inet.h>
#endif

CAF_PUSH_WARNINGS
#include "pingpong.pb.h"
CAF_POP_WARNINGS
CAF_BEGIN_TYPE_ID_BLOCK(pingpong_example, first_custom_type_id)

CAF_ADD_ATOM(pingpong_example, kickoff_atom)

CAF_END_TYPE_ID_BLOCK(pingpong_example)

// utility function to print an exit message with custom name
void print_on_exit(caf::scheduled_actor* self, const std::string& name) {
  self->attach_functor([=](const caf::error& reason) {
    aout(self) << name << " exited: " << to_string(reason) << std::endl;
  });
}


void protobuf_io(caf::io::broker* self, caf::io::connection_handle hdl, const caf::actor& buddy) {
  print_on_exit(self, "protobuf_io");
  aout(self) << "protobuf broker started" << std::endl;
  self->monitor(buddy);
  self->set_down_handler([=](const caf::down_msg& dm) {
    if (dm.source == buddy) {
      aout(self) << "our buddy is down" << std::endl;
      self->quit(dm.reason);
    }
  });
  auto write = [=](const org::caf::PingOrPong& p) {
    std::string buf = p.SerializeAsString();
    auto s = htonl(static_cast<uint32_t>(buf.size()));
    self->write(hdl, sizeof(uint32_t), &s);
    self->write(hdl, buf.size(), buf.data());
    self->flush(hdl);
  };
  auto default_callbacks = caf::message_handler{
      [=](const caf::io::connection_closed_msg&) {
        aout(self) << "connection closed" << std::endl;
        self->send_exit(buddy, caf::exit_reason::remote_link_unreachable);
        self->quit(caf::exit_reason::remote_link_unreachable);
      },
      [=](caf::ping_atom, int i) {
        aout(self) << "'ping' " << i << std::endl;
        org::caf::PingOrPong p;
        p.mutable_ping()->set_id(i);
        write(p);
      },
      [=](caf::pong_atom, int i) {
        aout(self) << "'pong' " << i << std::endl;
        org::caf::PingOrPong p;
        p.mutable_pong()->set_id(i);
        write(p);
      },
  };
  auto await_protobuf_data = caf::message_handler {
      [=](const caf::io::new_data_msg& msg) {
        org::caf::PingOrPong p;
        p.ParseFromArray(msg.buf.data(), static_cast<int>(msg.buf.size()));
        if (p.has_ping()) {
          self->send(buddy, caf::ping_atom_v, p.ping().id());
        }
        else if (p.has_pong()) {
          self->send(buddy, caf::pong_atom_v, p.pong().id());
        }
        else {
          self->quit(caf::exit_reason::user_shutdown);
          std::cerr << "neither Ping nor Pong!" << std::endl;
        }
        // receive next length prefix
        self->configure_read(hdl, caf::io::receive_policy::exactly(sizeof(uint32_t)));
        self->unbecome();
      },
  }.or_else(default_callbacks);
  auto await_length_prefix = caf::message_handler {
      [=](const caf::io::new_data_msg& msg) {
        uint32_t num_bytes;
        memcpy(&num_bytes, msg.buf.data(), sizeof(uint32_t));
        num_bytes = htonl(num_bytes);
        if (num_bytes > (1024 * 1024)) {
          aout(self) << "someone is trying something nasty" << std::endl;
          self->quit(caf::exit_reason::user_shutdown);
          return;
        }
        // receive protobuf data
        auto nb = static_cast<size_t>(num_bytes);
        self->configure_read(hdl, caf::io::receive_policy::exactly(nb));
        self->become(caf::keep_behavior, await_protobuf_data);
      },
  }.or_else(default_callbacks);
  // initial setup
  self->configure_read(hdl, caf::io::receive_policy::exactly(sizeof(uint32_t)));
  self->become(await_length_prefix);
}

#endif //EXAMPLE_ECS_INCLUDE_PROTO_TYPE_ID_H_

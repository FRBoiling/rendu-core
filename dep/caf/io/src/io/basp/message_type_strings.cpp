#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/io/basp/message_type.hpp"

#include <string>
#include <string_view>

namespace caf::io::basp {

std::string to_string(message_type x) {
  switch (x) {
    default:
      return "???";
    case message_type::server_handshake:
      return "caf::io::basp::message_type::server_handshake";
    case message_type::client_handshake:
      return "caf::io::basp::message_type::client_handshake";
    case message_type::direct_message:
      return "caf::io::basp::message_type::direct_message";
    case message_type::routed_message:
      return "caf::io::basp::message_type::routed_message";
    case message_type::monitor_message:
      return "caf::io::basp::message_type::monitor_message";
    case message_type::down_message:
      return "caf::io::basp::message_type::down_message";
    case message_type::heartbeat:
      return "caf::io::basp::message_type::heartbeat";
  }
}

bool from_string(std::string_view in, message_type& out) {
  if (in == "caf::io::basp::message_type::server_handshake") {
    out = message_type::server_handshake;
    return true;
  }
  if (in == "caf::io::basp::message_type::client_handshake") {
    out = message_type::client_handshake;
    return true;
  }
  if (in == "caf::io::basp::message_type::direct_message") {
    out = message_type::direct_message;
    return true;
  }
  if (in == "caf::io::basp::message_type::routed_message") {
    out = message_type::routed_message;
    return true;
  }
  if (in == "caf::io::basp::message_type::monitor_message") {
    out = message_type::monitor_message;
    return true;
  }
  if (in == "caf::io::basp::message_type::down_message") {
    out = message_type::down_message;
    return true;
  }
  if (in == "caf::io::basp::message_type::heartbeat") {
    out = message_type::heartbeat;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<message_type> in,
                  message_type& out) {
  auto result = static_cast<message_type>(in);
  switch (result) {
    default:
      return false;
    case message_type::server_handshake:
    case message_type::client_handshake:
    case message_type::direct_message:
    case message_type::routed_message:
    case message_type::monitor_message:
    case message_type::down_message:
    case message_type::heartbeat:
      out = result;
      return true;
  }
}

} // namespace caf::io::basp


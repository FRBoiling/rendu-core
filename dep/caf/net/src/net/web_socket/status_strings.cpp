#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/net/web_socket/status.hpp"

#include <string>
#include <string_view>

namespace caf::net::web_socket {

std::string to_string(status x) {
  switch (x) {
    default:
      return "???";
    case status::normal_close:
      return "caf::net::web_socket::status::normal_close";
    case status::going_away:
      return "caf::net::web_socket::status::going_away";
    case status::protocol_error:
      return "caf::net::web_socket::status::protocol_error";
    case status::invalid_data:
      return "caf::net::web_socket::status::invalid_data";
    case status::no_status:
      return "caf::net::web_socket::status::no_status";
    case status::abnormal_exit:
      return "caf::net::web_socket::status::abnormal_exit";
    case status::inconsistent_data:
      return "caf::net::web_socket::status::inconsistent_data";
    case status::policy_violation:
      return "caf::net::web_socket::status::policy_violation";
    case status::message_too_big:
      return "caf::net::web_socket::status::message_too_big";
    case status::missing_extensions:
      return "caf::net::web_socket::status::missing_extensions";
    case status::unexpected_condition:
      return "caf::net::web_socket::status::unexpected_condition";
    case status::tls_handshake_failure:
      return "caf::net::web_socket::status::tls_handshake_failure";
  }
}

bool from_string(std::string_view in, status& out) {
  if (in == "caf::net::web_socket::status::normal_close") {
    out = status::normal_close;
    return true;
  }
  if (in == "caf::net::web_socket::status::going_away") {
    out = status::going_away;
    return true;
  }
  if (in == "caf::net::web_socket::status::protocol_error") {
    out = status::protocol_error;
    return true;
  }
  if (in == "caf::net::web_socket::status::invalid_data") {
    out = status::invalid_data;
    return true;
  }
  if (in == "caf::net::web_socket::status::no_status") {
    out = status::no_status;
    return true;
  }
  if (in == "caf::net::web_socket::status::abnormal_exit") {
    out = status::abnormal_exit;
    return true;
  }
  if (in == "caf::net::web_socket::status::inconsistent_data") {
    out = status::inconsistent_data;
    return true;
  }
  if (in == "caf::net::web_socket::status::policy_violation") {
    out = status::policy_violation;
    return true;
  }
  if (in == "caf::net::web_socket::status::message_too_big") {
    out = status::message_too_big;
    return true;
  }
  if (in == "caf::net::web_socket::status::missing_extensions") {
    out = status::missing_extensions;
    return true;
  }
  if (in == "caf::net::web_socket::status::unexpected_condition") {
    out = status::unexpected_condition;
    return true;
  }
  if (in == "caf::net::web_socket::status::tls_handshake_failure") {
    out = status::tls_handshake_failure;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<status> in,
                  status& out) {
  auto result = static_cast<status>(in);
  switch (result) {
    default:
      return false;
    case status::normal_close:
    case status::going_away:
    case status::protocol_error:
    case status::invalid_data:
    case status::no_status:
    case status::abnormal_exit:
    case status::inconsistent_data:
    case status::policy_violation:
    case status::message_too_big:
    case status::missing_extensions:
    case status::unexpected_condition:
    case status::tls_handshake_failure:
      out = result;
      return true;
  }
}

} // namespace caf::net::web_socket


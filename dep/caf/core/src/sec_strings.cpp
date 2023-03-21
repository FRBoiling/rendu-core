#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/sec.hpp"

#include <string>
#include <string_view>

namespace caf {

std::string to_string(sec x) {
  switch (x) {
    default:
      return "???";
    case sec::none:
      return "caf::sec::none";
    case sec::unexpected_message:
      return "caf::sec::unexpected_message";
    case sec::unexpected_response:
      return "caf::sec::unexpected_response";
    case sec::request_receiver_down:
      return "caf::sec::request_receiver_down";
    case sec::request_timeout:
      return "caf::sec::request_timeout";
    case sec::no_such_group_module:
      return "caf::sec::no_such_group_module";
    case sec::no_actor_published_at_port:
      return "caf::sec::no_actor_published_at_port";
    case sec::unexpected_actor_messaging_interface:
      return "caf::sec::unexpected_actor_messaging_interface";
    case sec::state_not_serializable:
      return "caf::sec::state_not_serializable";
    case sec::unsupported_sys_key:
      return "caf::sec::unsupported_sys_key";
    case sec::unsupported_sys_message:
      return "caf::sec::unsupported_sys_message";
    case sec::disconnect_during_handshake:
      return "caf::sec::disconnect_during_handshake";
    case sec::cannot_forward_to_invalid_actor:
      return "caf::sec::cannot_forward_to_invalid_actor";
    case sec::no_route_to_receiving_node:
      return "caf::sec::no_route_to_receiving_node";
    case sec::failed_to_assign_scribe_from_handle:
      return "caf::sec::failed_to_assign_scribe_from_handle";
    case sec::failed_to_assign_doorman_from_handle:
      return "caf::sec::failed_to_assign_doorman_from_handle";
    case sec::cannot_close_invalid_port:
      return "caf::sec::cannot_close_invalid_port";
    case sec::cannot_connect_to_node:
      return "caf::sec::cannot_connect_to_node";
    case sec::cannot_open_port:
      return "caf::sec::cannot_open_port";
    case sec::network_syscall_failed:
      return "caf::sec::network_syscall_failed";
    case sec::invalid_argument:
      return "caf::sec::invalid_argument";
    case sec::invalid_protocol_family:
      return "caf::sec::invalid_protocol_family";
    case sec::cannot_publish_invalid_actor:
      return "caf::sec::cannot_publish_invalid_actor";
    case sec::cannot_spawn_actor_from_arguments:
      return "caf::sec::cannot_spawn_actor_from_arguments";
    case sec::end_of_stream:
      return "caf::sec::end_of_stream";
    case sec::no_context:
      return "caf::sec::no_context";
    case sec::unknown_type:
      return "caf::sec::unknown_type";
    case sec::no_proxy_registry:
      return "caf::sec::no_proxy_registry";
    case sec::runtime_error:
      return "caf::sec::runtime_error";
    case sec::remote_linking_failed:
      return "caf::sec::remote_linking_failed";
    case sec::invalid_stream:
      return "caf::sec::invalid_stream";
    case sec::cannot_resubscribe_stream:
      return "caf::sec::cannot_resubscribe_stream";
    case sec::bad_function_call:
      return "caf::sec::bad_function_call";
    case sec::feature_disabled:
      return "caf::sec::feature_disabled";
    case sec::cannot_open_file:
      return "caf::sec::cannot_open_file";
    case sec::socket_invalid:
      return "caf::sec::socket_invalid";
    case sec::socket_disconnected:
      return "caf::sec::socket_disconnected";
    case sec::socket_operation_failed:
      return "caf::sec::socket_operation_failed";
    case sec::unavailable_or_would_block:
      return "caf::sec::unavailable_or_would_block";
    case sec::incompatible_versions:
      return "caf::sec::incompatible_versions";
    case sec::incompatible_application_ids:
      return "caf::sec::incompatible_application_ids";
    case sec::malformed_basp_message:
      return "caf::sec::malformed_basp_message";
    case sec::serializing_basp_payload_failed:
      return "caf::sec::serializing_basp_payload_failed";
    case sec::redundant_connection:
      return "caf::sec::redundant_connection";
    case sec::remote_lookup_failed:
      return "caf::sec::remote_lookup_failed";
    case sec::no_tracing_context:
      return "caf::sec::no_tracing_context";
    case sec::all_requests_failed:
      return "caf::sec::all_requests_failed";
    case sec::field_invariant_check_failed:
      return "caf::sec::field_invariant_check_failed";
    case sec::field_value_synchronization_failed:
      return "caf::sec::field_value_synchronization_failed";
    case sec::invalid_field_type:
      return "caf::sec::invalid_field_type";
    case sec::unsafe_type:
      return "caf::sec::unsafe_type";
    case sec::save_callback_failed:
      return "caf::sec::save_callback_failed";
    case sec::load_callback_failed:
      return "caf::sec::load_callback_failed";
    case sec::conversion_failed:
      return "caf::sec::conversion_failed";
    case sec::connection_closed:
      return "caf::sec::connection_closed";
    case sec::type_clash:
      return "caf::sec::type_clash";
    case sec::unsupported_operation:
      return "caf::sec::unsupported_operation";
    case sec::no_such_key:
      return "caf::sec::no_such_key";
    case sec::broken_promise:
      return "caf::sec::broken_promise";
    case sec::connection_timeout:
      return "caf::sec::connection_timeout";
    case sec::action_reschedule_failed:
      return "caf::sec::action_reschedule_failed";
    case sec::invalid_observable:
      return "caf::sec::invalid_observable";
    case sec::too_many_observers:
      return "caf::sec::too_many_observers";
    case sec::disposed:
      return "caf::sec::disposed";
    case sec::cannot_open_resource:
      return "caf::sec::cannot_open_resource";
    case sec::protocol_error:
      return "caf::sec::protocol_error";
    case sec::logic_error:
      return "caf::sec::logic_error";
  }
}

bool from_string(std::string_view in, sec& out) {
  if (in == "caf::sec::none") {
    out = sec::none;
    return true;
  }
  if (in == "caf::sec::unexpected_message") {
    out = sec::unexpected_message;
    return true;
  }
  if (in == "caf::sec::unexpected_response") {
    out = sec::unexpected_response;
    return true;
  }
  if (in == "caf::sec::request_receiver_down") {
    out = sec::request_receiver_down;
    return true;
  }
  if (in == "caf::sec::request_timeout") {
    out = sec::request_timeout;
    return true;
  }
  if (in == "caf::sec::no_such_group_module") {
    out = sec::no_such_group_module;
    return true;
  }
  if (in == "caf::sec::no_actor_published_at_port") {
    out = sec::no_actor_published_at_port;
    return true;
  }
  if (in == "caf::sec::unexpected_actor_messaging_interface") {
    out = sec::unexpected_actor_messaging_interface;
    return true;
  }
  if (in == "caf::sec::state_not_serializable") {
    out = sec::state_not_serializable;
    return true;
  }
  if (in == "caf::sec::unsupported_sys_key") {
    out = sec::unsupported_sys_key;
    return true;
  }
  if (in == "caf::sec::unsupported_sys_message") {
    out = sec::unsupported_sys_message;
    return true;
  }
  if (in == "caf::sec::disconnect_during_handshake") {
    out = sec::disconnect_during_handshake;
    return true;
  }
  if (in == "caf::sec::cannot_forward_to_invalid_actor") {
    out = sec::cannot_forward_to_invalid_actor;
    return true;
  }
  if (in == "caf::sec::no_route_to_receiving_node") {
    out = sec::no_route_to_receiving_node;
    return true;
  }
  if (in == "caf::sec::failed_to_assign_scribe_from_handle") {
    out = sec::failed_to_assign_scribe_from_handle;
    return true;
  }
  if (in == "caf::sec::failed_to_assign_doorman_from_handle") {
    out = sec::failed_to_assign_doorman_from_handle;
    return true;
  }
  if (in == "caf::sec::cannot_close_invalid_port") {
    out = sec::cannot_close_invalid_port;
    return true;
  }
  if (in == "caf::sec::cannot_connect_to_node") {
    out = sec::cannot_connect_to_node;
    return true;
  }
  if (in == "caf::sec::cannot_open_port") {
    out = sec::cannot_open_port;
    return true;
  }
  if (in == "caf::sec::network_syscall_failed") {
    out = sec::network_syscall_failed;
    return true;
  }
  if (in == "caf::sec::invalid_argument") {
    out = sec::invalid_argument;
    return true;
  }
  if (in == "caf::sec::invalid_protocol_family") {
    out = sec::invalid_protocol_family;
    return true;
  }
  if (in == "caf::sec::cannot_publish_invalid_actor") {
    out = sec::cannot_publish_invalid_actor;
    return true;
  }
  if (in == "caf::sec::cannot_spawn_actor_from_arguments") {
    out = sec::cannot_spawn_actor_from_arguments;
    return true;
  }
  if (in == "caf::sec::end_of_stream") {
    out = sec::end_of_stream;
    return true;
  }
  if (in == "caf::sec::no_context") {
    out = sec::no_context;
    return true;
  }
  if (in == "caf::sec::unknown_type") {
    out = sec::unknown_type;
    return true;
  }
  if (in == "caf::sec::no_proxy_registry") {
    out = sec::no_proxy_registry;
    return true;
  }
  if (in == "caf::sec::runtime_error") {
    out = sec::runtime_error;
    return true;
  }
  if (in == "caf::sec::remote_linking_failed") {
    out = sec::remote_linking_failed;
    return true;
  }
  if (in == "caf::sec::invalid_stream") {
    out = sec::invalid_stream;
    return true;
  }
  if (in == "caf::sec::cannot_resubscribe_stream") {
    out = sec::cannot_resubscribe_stream;
    return true;
  }
  if (in == "caf::sec::bad_function_call") {
    out = sec::bad_function_call;
    return true;
  }
  if (in == "caf::sec::feature_disabled") {
    out = sec::feature_disabled;
    return true;
  }
  if (in == "caf::sec::cannot_open_file") {
    out = sec::cannot_open_file;
    return true;
  }
  if (in == "caf::sec::socket_invalid") {
    out = sec::socket_invalid;
    return true;
  }
  if (in == "caf::sec::socket_disconnected") {
    out = sec::socket_disconnected;
    return true;
  }
  if (in == "caf::sec::socket_operation_failed") {
    out = sec::socket_operation_failed;
    return true;
  }
  if (in == "caf::sec::unavailable_or_would_block") {
    out = sec::unavailable_or_would_block;
    return true;
  }
  if (in == "caf::sec::incompatible_versions") {
    out = sec::incompatible_versions;
    return true;
  }
  if (in == "caf::sec::incompatible_application_ids") {
    out = sec::incompatible_application_ids;
    return true;
  }
  if (in == "caf::sec::malformed_basp_message") {
    out = sec::malformed_basp_message;
    return true;
  }
  if (in == "caf::sec::serializing_basp_payload_failed") {
    out = sec::serializing_basp_payload_failed;
    return true;
  }
  if (in == "caf::sec::redundant_connection") {
    out = sec::redundant_connection;
    return true;
  }
  if (in == "caf::sec::remote_lookup_failed") {
    out = sec::remote_lookup_failed;
    return true;
  }
  if (in == "caf::sec::no_tracing_context") {
    out = sec::no_tracing_context;
    return true;
  }
  if (in == "caf::sec::all_requests_failed") {
    out = sec::all_requests_failed;
    return true;
  }
  if (in == "caf::sec::field_invariant_check_failed") {
    out = sec::field_invariant_check_failed;
    return true;
  }
  if (in == "caf::sec::field_value_synchronization_failed") {
    out = sec::field_value_synchronization_failed;
    return true;
  }
  if (in == "caf::sec::invalid_field_type") {
    out = sec::invalid_field_type;
    return true;
  }
  if (in == "caf::sec::unsafe_type") {
    out = sec::unsafe_type;
    return true;
  }
  if (in == "caf::sec::save_callback_failed") {
    out = sec::save_callback_failed;
    return true;
  }
  if (in == "caf::sec::load_callback_failed") {
    out = sec::load_callback_failed;
    return true;
  }
  if (in == "caf::sec::conversion_failed") {
    out = sec::conversion_failed;
    return true;
  }
  if (in == "caf::sec::connection_closed") {
    out = sec::connection_closed;
    return true;
  }
  if (in == "caf::sec::type_clash") {
    out = sec::type_clash;
    return true;
  }
  if (in == "caf::sec::unsupported_operation") {
    out = sec::unsupported_operation;
    return true;
  }
  if (in == "caf::sec::no_such_key") {
    out = sec::no_such_key;
    return true;
  }
  if (in == "caf::sec::broken_promise") {
    out = sec::broken_promise;
    return true;
  }
  if (in == "caf::sec::connection_timeout") {
    out = sec::connection_timeout;
    return true;
  }
  if (in == "caf::sec::action_reschedule_failed") {
    out = sec::action_reschedule_failed;
    return true;
  }
  if (in == "caf::sec::invalid_observable") {
    out = sec::invalid_observable;
    return true;
  }
  if (in == "caf::sec::too_many_observers") {
    out = sec::too_many_observers;
    return true;
  }
  if (in == "caf::sec::disposed") {
    out = sec::disposed;
    return true;
  }
  if (in == "caf::sec::cannot_open_resource") {
    out = sec::cannot_open_resource;
    return true;
  }
  if (in == "caf::sec::protocol_error") {
    out = sec::protocol_error;
    return true;
  }
  if (in == "caf::sec::logic_error") {
    out = sec::logic_error;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<sec> in,
                  sec& out) {
  auto result = static_cast<sec>(in);
  switch (result) {
    default:
      return false;
    case sec::none:
    case sec::unexpected_message:
    case sec::unexpected_response:
    case sec::request_receiver_down:
    case sec::request_timeout:
    case sec::no_such_group_module:
    case sec::no_actor_published_at_port:
    case sec::unexpected_actor_messaging_interface:
    case sec::state_not_serializable:
    case sec::unsupported_sys_key:
    case sec::unsupported_sys_message:
    case sec::disconnect_during_handshake:
    case sec::cannot_forward_to_invalid_actor:
    case sec::no_route_to_receiving_node:
    case sec::failed_to_assign_scribe_from_handle:
    case sec::failed_to_assign_doorman_from_handle:
    case sec::cannot_close_invalid_port:
    case sec::cannot_connect_to_node:
    case sec::cannot_open_port:
    case sec::network_syscall_failed:
    case sec::invalid_argument:
    case sec::invalid_protocol_family:
    case sec::cannot_publish_invalid_actor:
    case sec::cannot_spawn_actor_from_arguments:
    case sec::end_of_stream:
    case sec::no_context:
    case sec::unknown_type:
    case sec::no_proxy_registry:
    case sec::runtime_error:
    case sec::remote_linking_failed:
    case sec::invalid_stream:
    case sec::cannot_resubscribe_stream:
    case sec::bad_function_call:
    case sec::feature_disabled:
    case sec::cannot_open_file:
    case sec::socket_invalid:
    case sec::socket_disconnected:
    case sec::socket_operation_failed:
    case sec::unavailable_or_would_block:
    case sec::incompatible_versions:
    case sec::incompatible_application_ids:
    case sec::malformed_basp_message:
    case sec::serializing_basp_payload_failed:
    case sec::redundant_connection:
    case sec::remote_lookup_failed:
    case sec::no_tracing_context:
    case sec::all_requests_failed:
    case sec::field_invariant_check_failed:
    case sec::field_value_synchronization_failed:
    case sec::invalid_field_type:
    case sec::unsafe_type:
    case sec::save_callback_failed:
    case sec::load_callback_failed:
    case sec::conversion_failed:
    case sec::connection_closed:
    case sec::type_clash:
    case sec::unsupported_operation:
    case sec::no_such_key:
    case sec::broken_promise:
    case sec::connection_timeout:
    case sec::action_reschedule_failed:
    case sec::invalid_observable:
    case sec::too_many_observers:
    case sec::disposed:
    case sec::cannot_open_resource:
    case sec::protocol_error:
    case sec::logic_error:
      out = result;
      return true;
  }
}

} // namespace caf


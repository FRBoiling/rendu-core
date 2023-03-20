#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/net/http/status.hpp"

#include <string>
#include <string_view>

namespace caf::net::http {

std::string to_string(status x) {
  switch (x) {
    default:
      return "???";
    case status::continue_request:
      return "caf::net::http::status::continue_request";
    case status::switching_protocols:
      return "caf::net::http::status::switching_protocols";
    case status::ok:
      return "caf::net::http::status::ok";
    case status::created:
      return "caf::net::http::status::created";
    case status::accepted:
      return "caf::net::http::status::accepted";
    case status::non_authoritative_information:
      return "caf::net::http::status::non_authoritative_information";
    case status::no_content:
      return "caf::net::http::status::no_content";
    case status::reset_content:
      return "caf::net::http::status::reset_content";
    case status::partial_content:
      return "caf::net::http::status::partial_content";
    case status::multiple_choices:
      return "caf::net::http::status::multiple_choices";
    case status::moved_permanently:
      return "caf::net::http::status::moved_permanently";
    case status::found:
      return "caf::net::http::status::found";
    case status::see_other:
      return "caf::net::http::status::see_other";
    case status::not_modified:
      return "caf::net::http::status::not_modified";
    case status::use_proxy:
      return "caf::net::http::status::use_proxy";
    case status::temporary_redirect:
      return "caf::net::http::status::temporary_redirect";
    case status::bad_request:
      return "caf::net::http::status::bad_request";
    case status::unauthorized:
      return "caf::net::http::status::unauthorized";
    case status::payment_required:
      return "caf::net::http::status::payment_required";
    case status::forbidden:
      return "caf::net::http::status::forbidden";
    case status::not_found:
      return "caf::net::http::status::not_found";
    case status::method_not_allowed:
      return "caf::net::http::status::method_not_allowed";
    case status::not_acceptable:
      return "caf::net::http::status::not_acceptable";
    case status::proxy_authentication_required:
      return "caf::net::http::status::proxy_authentication_required";
    case status::request_timeout:
      return "caf::net::http::status::request_timeout";
    case status::conflict:
      return "caf::net::http::status::conflict";
    case status::gone:
      return "caf::net::http::status::gone";
    case status::length_required:
      return "caf::net::http::status::length_required";
    case status::precondition_failed:
      return "caf::net::http::status::precondition_failed";
    case status::payload_too_large:
      return "caf::net::http::status::payload_too_large";
    case status::uri_too_long:
      return "caf::net::http::status::uri_too_long";
    case status::unsupported_media_type:
      return "caf::net::http::status::unsupported_media_type";
    case status::range_not_satisfiable:
      return "caf::net::http::status::range_not_satisfiable";
    case status::expectation_failed:
      return "caf::net::http::status::expectation_failed";
    case status::upgrade_required:
      return "caf::net::http::status::upgrade_required";
    case status::precondition_required:
      return "caf::net::http::status::precondition_required";
    case status::too_many_requests:
      return "caf::net::http::status::too_many_requests";
    case status::request_header_fields_too_large:
      return "caf::net::http::status::request_header_fields_too_large";
    case status::internal_server_error:
      return "caf::net::http::status::internal_server_error";
    case status::not_implemented:
      return "caf::net::http::status::not_implemented";
    case status::bad_gateway:
      return "caf::net::http::status::bad_gateway";
    case status::service_unavailable:
      return "caf::net::http::status::service_unavailable";
    case status::gateway_timeout:
      return "caf::net::http::status::gateway_timeout";
    case status::http_version_not_supported:
      return "caf::net::http::status::http_version_not_supported";
    case status::network_authentication_required:
      return "caf::net::http::status::network_authentication_required";
  }
}

bool from_string(std::string_view in, status& out) {
  if (in == "caf::net::http::status::continue_request") {
    out = status::continue_request;
    return true;
  }
  if (in == "caf::net::http::status::switching_protocols") {
    out = status::switching_protocols;
    return true;
  }
  if (in == "caf::net::http::status::ok") {
    out = status::ok;
    return true;
  }
  if (in == "caf::net::http::status::created") {
    out = status::created;
    return true;
  }
  if (in == "caf::net::http::status::accepted") {
    out = status::accepted;
    return true;
  }
  if (in == "caf::net::http::status::non_authoritative_information") {
    out = status::non_authoritative_information;
    return true;
  }
  if (in == "caf::net::http::status::no_content") {
    out = status::no_content;
    return true;
  }
  if (in == "caf::net::http::status::reset_content") {
    out = status::reset_content;
    return true;
  }
  if (in == "caf::net::http::status::partial_content") {
    out = status::partial_content;
    return true;
  }
  if (in == "caf::net::http::status::multiple_choices") {
    out = status::multiple_choices;
    return true;
  }
  if (in == "caf::net::http::status::moved_permanently") {
    out = status::moved_permanently;
    return true;
  }
  if (in == "caf::net::http::status::found") {
    out = status::found;
    return true;
  }
  if (in == "caf::net::http::status::see_other") {
    out = status::see_other;
    return true;
  }
  if (in == "caf::net::http::status::not_modified") {
    out = status::not_modified;
    return true;
  }
  if (in == "caf::net::http::status::use_proxy") {
    out = status::use_proxy;
    return true;
  }
  if (in == "caf::net::http::status::temporary_redirect") {
    out = status::temporary_redirect;
    return true;
  }
  if (in == "caf::net::http::status::bad_request") {
    out = status::bad_request;
    return true;
  }
  if (in == "caf::net::http::status::unauthorized") {
    out = status::unauthorized;
    return true;
  }
  if (in == "caf::net::http::status::payment_required") {
    out = status::payment_required;
    return true;
  }
  if (in == "caf::net::http::status::forbidden") {
    out = status::forbidden;
    return true;
  }
  if (in == "caf::net::http::status::not_found") {
    out = status::not_found;
    return true;
  }
  if (in == "caf::net::http::status::method_not_allowed") {
    out = status::method_not_allowed;
    return true;
  }
  if (in == "caf::net::http::status::not_acceptable") {
    out = status::not_acceptable;
    return true;
  }
  if (in == "caf::net::http::status::proxy_authentication_required") {
    out = status::proxy_authentication_required;
    return true;
  }
  if (in == "caf::net::http::status::request_timeout") {
    out = status::request_timeout;
    return true;
  }
  if (in == "caf::net::http::status::conflict") {
    out = status::conflict;
    return true;
  }
  if (in == "caf::net::http::status::gone") {
    out = status::gone;
    return true;
  }
  if (in == "caf::net::http::status::length_required") {
    out = status::length_required;
    return true;
  }
  if (in == "caf::net::http::status::precondition_failed") {
    out = status::precondition_failed;
    return true;
  }
  if (in == "caf::net::http::status::payload_too_large") {
    out = status::payload_too_large;
    return true;
  }
  if (in == "caf::net::http::status::uri_too_long") {
    out = status::uri_too_long;
    return true;
  }
  if (in == "caf::net::http::status::unsupported_media_type") {
    out = status::unsupported_media_type;
    return true;
  }
  if (in == "caf::net::http::status::range_not_satisfiable") {
    out = status::range_not_satisfiable;
    return true;
  }
  if (in == "caf::net::http::status::expectation_failed") {
    out = status::expectation_failed;
    return true;
  }
  if (in == "caf::net::http::status::upgrade_required") {
    out = status::upgrade_required;
    return true;
  }
  if (in == "caf::net::http::status::precondition_required") {
    out = status::precondition_required;
    return true;
  }
  if (in == "caf::net::http::status::too_many_requests") {
    out = status::too_many_requests;
    return true;
  }
  if (in == "caf::net::http::status::request_header_fields_too_large") {
    out = status::request_header_fields_too_large;
    return true;
  }
  if (in == "caf::net::http::status::internal_server_error") {
    out = status::internal_server_error;
    return true;
  }
  if (in == "caf::net::http::status::not_implemented") {
    out = status::not_implemented;
    return true;
  }
  if (in == "caf::net::http::status::bad_gateway") {
    out = status::bad_gateway;
    return true;
  }
  if (in == "caf::net::http::status::service_unavailable") {
    out = status::service_unavailable;
    return true;
  }
  if (in == "caf::net::http::status::gateway_timeout") {
    out = status::gateway_timeout;
    return true;
  }
  if (in == "caf::net::http::status::http_version_not_supported") {
    out = status::http_version_not_supported;
    return true;
  }
  if (in == "caf::net::http::status::network_authentication_required") {
    out = status::network_authentication_required;
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
    case status::continue_request:
    case status::switching_protocols:
    case status::ok:
    case status::created:
    case status::accepted:
    case status::non_authoritative_information:
    case status::no_content:
    case status::reset_content:
    case status::partial_content:
    case status::multiple_choices:
    case status::moved_permanently:
    case status::found:
    case status::see_other:
    case status::not_modified:
    case status::use_proxy:
    case status::temporary_redirect:
    case status::bad_request:
    case status::unauthorized:
    case status::payment_required:
    case status::forbidden:
    case status::not_found:
    case status::method_not_allowed:
    case status::not_acceptable:
    case status::proxy_authentication_required:
    case status::request_timeout:
    case status::conflict:
    case status::gone:
    case status::length_required:
    case status::precondition_failed:
    case status::payload_too_large:
    case status::uri_too_long:
    case status::unsupported_media_type:
    case status::range_not_satisfiable:
    case status::expectation_failed:
    case status::upgrade_required:
    case status::precondition_required:
    case status::too_many_requests:
    case status::request_header_fields_too_large:
    case status::internal_server_error:
    case status::not_implemented:
    case status::bad_gateway:
    case status::service_unavailable:
    case status::gateway_timeout:
    case status::http_version_not_supported:
    case status::network_authentication_required:
      out = result;
      return true;
  }
}

} // namespace caf::net::http


#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/net/http/method.hpp"

#include <string>
#include <string_view>

namespace caf::net::http {

std::string to_string(method x) {
  switch (x) {
    default:
      return "???";
    case method::get:
      return "caf::net::http::method::get";
    case method::head:
      return "caf::net::http::method::head";
    case method::post:
      return "caf::net::http::method::post";
    case method::put:
      return "caf::net::http::method::put";
    case method::del:
      return "caf::net::http::method::del";
    case method::connect:
      return "caf::net::http::method::connect";
    case method::options:
      return "caf::net::http::method::options";
    case method::trace:
      return "caf::net::http::method::trace";
  }
}

bool from_string(std::string_view in, method& out) {
  if (in == "caf::net::http::method::get") {
    out = method::get;
    return true;
  }
  if (in == "caf::net::http::method::head") {
    out = method::head;
    return true;
  }
  if (in == "caf::net::http::method::post") {
    out = method::post;
    return true;
  }
  if (in == "caf::net::http::method::put") {
    out = method::put;
    return true;
  }
  if (in == "caf::net::http::method::del") {
    out = method::del;
    return true;
  }
  if (in == "caf::net::http::method::connect") {
    out = method::connect;
    return true;
  }
  if (in == "caf::net::http::method::options") {
    out = method::options;
    return true;
  }
  if (in == "caf::net::http::method::trace") {
    out = method::trace;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<method> in,
                  method& out) {
  auto result = static_cast<method>(in);
  switch (result) {
    default:
      return false;
    case method::get:
    case method::head:
    case method::post:
    case method::put:
    case method::del:
    case method::connect:
    case method::options:
    case method::trace:
      out = result;
      return true;
  }
}

} // namespace caf::net::http


#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/io/network/operation.hpp"

#include <string>
#include <string_view>

namespace caf::io::network {

std::string to_string(operation x) {
  switch (x) {
    default:
      return "???";
    case operation::read:
      return "caf::io::network::operation::read";
    case operation::write:
      return "caf::io::network::operation::write";
    case operation::propagate_error:
      return "caf::io::network::operation::propagate_error";
  }
}

bool from_string(std::string_view in, operation& out) {
  if (in == "caf::io::network::operation::read") {
    out = operation::read;
    return true;
  }
  if (in == "caf::io::network::operation::write") {
    out = operation::write;
    return true;
  }
  if (in == "caf::io::network::operation::propagate_error") {
    out = operation::propagate_error;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<operation> in,
                  operation& out) {
  auto result = static_cast<operation>(in);
  switch (result) {
    default:
      return false;
    case operation::read:
    case operation::write:
    case operation::propagate_error:
      out = result;
      return true;
  }
}

} // namespace caf::io::network


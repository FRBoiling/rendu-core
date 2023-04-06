#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/message_priority.hpp"

#include <string>
#include <string_view>

namespace caf {

std::string to_string(message_priority x) {
  switch (x) {
    default:
      return "???";
    case message_priority::high:
      return "caf::message_priority::high";
    case message_priority::normal:
      return "caf::message_priority::normal";
  }
}

bool from_string(std::string_view in, message_priority& out) {
  if (in == "caf::message_priority::high") {
    out = message_priority::high;
    return true;
  }
  if (in == "caf::message_priority::normal") {
    out = message_priority::normal;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<message_priority> in,
                  message_priority& out) {
  auto result = static_cast<message_priority>(in);
  switch (result) {
    default:
      return false;
    case message_priority::high:
    case message_priority::normal:
      out = result;
      return true;
  }
}

} // namespace caf


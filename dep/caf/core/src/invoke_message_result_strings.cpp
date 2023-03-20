#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/invoke_message_result.hpp"

#include <string>
#include <string_view>

namespace caf {

std::string to_string(invoke_message_result x) {
  switch (x) {
    default:
      return "???";
    case invoke_message_result::consumed:
      return "caf::invoke_message_result::consumed";
    case invoke_message_result::skipped:
      return "caf::invoke_message_result::skipped";
    case invoke_message_result::dropped:
      return "caf::invoke_message_result::dropped";
  }
}

bool from_string(std::string_view in, invoke_message_result& out) {
  if (in == "caf::invoke_message_result::consumed") {
    out = invoke_message_result::consumed;
    return true;
  }
  if (in == "caf::invoke_message_result::skipped") {
    out = invoke_message_result::skipped;
    return true;
  }
  if (in == "caf::invoke_message_result::dropped") {
    out = invoke_message_result::dropped;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<invoke_message_result> in,
                  invoke_message_result& out) {
  auto result = static_cast<invoke_message_result>(in);
  switch (result) {
    default:
      return false;
    case invoke_message_result::consumed:
    case invoke_message_result::skipped:
    case invoke_message_result::dropped:
      out = result;
      return true;
  }
}

} // namespace caf


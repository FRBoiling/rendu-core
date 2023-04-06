#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/intrusive/inbox_result.hpp"

#include <string>
#include <string_view>

namespace caf::intrusive {

std::string to_string(inbox_result x) {
  switch (x) {
    default:
      return "???";
    case inbox_result::success:
      return "caf::intrusive::inbox_result::success";
    case inbox_result::unblocked_reader:
      return "caf::intrusive::inbox_result::unblocked_reader";
    case inbox_result::queue_closed:
      return "caf::intrusive::inbox_result::queue_closed";
  }
}

bool from_string(std::string_view in, inbox_result& out) {
  if (in == "caf::intrusive::inbox_result::success") {
    out = inbox_result::success;
    return true;
  }
  if (in == "caf::intrusive::inbox_result::unblocked_reader") {
    out = inbox_result::unblocked_reader;
    return true;
  }
  if (in == "caf::intrusive::inbox_result::queue_closed") {
    out = inbox_result::queue_closed;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<inbox_result> in,
                  inbox_result& out) {
  auto result = static_cast<inbox_result>(in);
  switch (result) {
    default:
      return false;
    case inbox_result::success:
    case inbox_result::unblocked_reader:
    case inbox_result::queue_closed:
      out = result;
      return true;
  }
}

} // namespace caf::intrusive


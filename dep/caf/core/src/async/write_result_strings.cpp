#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/async/write_result.hpp"

#include <string>
#include <string_view>

namespace caf::async {

std::string to_string(write_result x) {
  switch (x) {
    default:
      return "???";
    case write_result::ok:
      return "caf::async::write_result::ok";
    case write_result::drop:
      return "caf::async::write_result::drop";
    case write_result::timeout:
      return "caf::async::write_result::timeout";
  }
}

bool from_string(std::string_view in, write_result& out) {
  if (in == "caf::async::write_result::ok") {
    out = write_result::ok;
    return true;
  }
  if (in == "caf::async::write_result::drop") {
    out = write_result::drop;
    return true;
  }
  if (in == "caf::async::write_result::timeout") {
    out = write_result::timeout;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<write_result> in,
                  write_result& out) {
  auto result = static_cast<write_result>(in);
  switch (result) {
    default:
      return false;
    case write_result::ok:
    case write_result::drop:
    case write_result::timeout:
      out = result;
      return true;
  }
}

} // namespace caf::async


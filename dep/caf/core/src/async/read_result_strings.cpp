#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/async/read_result.hpp"

#include <string>
#include <string_view>

namespace caf::async {

std::string to_string(read_result x) {
  switch (x) {
    default:
      return "???";
    case read_result::ok:
      return "caf::async::read_result::ok";
    case read_result::stop:
      return "caf::async::read_result::stop";
    case read_result::abort:
      return "caf::async::read_result::abort";
    case read_result::try_again_later:
      return "caf::async::read_result::try_again_later";
  }
}

bool from_string(std::string_view in, read_result& out) {
  if (in == "caf::async::read_result::ok") {
    out = read_result::ok;
    return true;
  }
  if (in == "caf::async::read_result::stop") {
    out = read_result::stop;
    return true;
  }
  if (in == "caf::async::read_result::abort") {
    out = read_result::abort;
    return true;
  }
  if (in == "caf::async::read_result::try_again_later") {
    out = read_result::try_again_later;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<read_result> in,
                  read_result& out) {
  auto result = static_cast<read_result>(in);
  switch (result) {
    default:
      return false;
    case read_result::ok:
    case read_result::stop:
    case read_result::abort:
    case read_result::try_again_later:
      out = result;
      return true;
  }
}

} // namespace caf::async


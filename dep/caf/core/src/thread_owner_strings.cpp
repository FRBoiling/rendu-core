#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/thread_owner.hpp"

#include <string>
#include <string_view>

namespace caf {

std::string to_string(thread_owner x) {
  switch (x) {
    default:
      return "???";
    case thread_owner::scheduler:
      return "caf::thread_owner::scheduler";
    case thread_owner::pool:
      return "caf::thread_owner::pool";
    case thread_owner::system:
      return "caf::thread_owner::system";
    case thread_owner::other:
      return "caf::thread_owner::other";
  }
}

bool from_string(std::string_view in, thread_owner& out) {
  if (in == "caf::thread_owner::scheduler") {
    out = thread_owner::scheduler;
    return true;
  }
  if (in == "caf::thread_owner::pool") {
    out = thread_owner::pool;
    return true;
  }
  if (in == "caf::thread_owner::system") {
    out = thread_owner::system;
    return true;
  }
  if (in == "caf::thread_owner::other") {
    out = thread_owner::other;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<thread_owner> in,
                  thread_owner& out) {
  auto result = static_cast<thread_owner>(in);
  switch (result) {
    default:
      return false;
    case thread_owner::scheduler:
    case thread_owner::pool:
    case thread_owner::system:
    case thread_owner::other:
      out = result;
      return true;
  }
}

} // namespace caf


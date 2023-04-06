#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/intrusive/task_result.hpp"

#include <string>
#include <string_view>

namespace caf::intrusive {

std::string to_string(task_result x) {
  switch (x) {
    default:
      return "???";
    case task_result::resume:
      return "caf::intrusive::task_result::resume";
    case task_result::skip:
      return "caf::intrusive::task_result::skip";
    case task_result::stop:
      return "caf::intrusive::task_result::stop";
    case task_result::stop_all:
      return "caf::intrusive::task_result::stop_all";
  }
}

bool from_string(std::string_view in, task_result& out) {
  if (in == "caf::intrusive::task_result::resume") {
    out = task_result::resume;
    return true;
  }
  if (in == "caf::intrusive::task_result::skip") {
    out = task_result::skip;
    return true;
  }
  if (in == "caf::intrusive::task_result::stop") {
    out = task_result::stop;
    return true;
  }
  if (in == "caf::intrusive::task_result::stop_all") {
    out = task_result::stop_all;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<task_result> in,
                  task_result& out) {
  auto result = static_cast<task_result>(in);
  switch (result) {
    default:
      return false;
    case task_result::resume:
    case task_result::skip:
    case task_result::stop:
    case task_result::stop_all:
      out = result;
      return true;
  }
}

} // namespace caf::intrusive


#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/flow/op/state.hpp"

#include <string>
#include <string_view>

namespace caf::flow::op {

std::string to_string(state x) {
  switch (x) {
    default:
      return "???";
    case state::idle:
      return "caf::flow::op::state::idle";
    case state::running:
      return "caf::flow::op::state::running";
    case state::completed:
      return "caf::flow::op::state::completed";
    case state::aborted:
      return "caf::flow::op::state::aborted";
    case state::disposed:
      return "caf::flow::op::state::disposed";
  }
}

bool from_string(std::string_view in, state& out) {
  if (in == "caf::flow::op::state::idle") {
    out = state::idle;
    return true;
  }
  if (in == "caf::flow::op::state::running") {
    out = state::running;
    return true;
  }
  if (in == "caf::flow::op::state::completed") {
    out = state::completed;
    return true;
  }
  if (in == "caf::flow::op::state::aborted") {
    out = state::aborted;
    return true;
  }
  if (in == "caf::flow::op::state::disposed") {
    out = state::disposed;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<state> in,
                  state& out) {
  auto result = static_cast<state>(in);
  switch (result) {
    default:
      return false;
    case state::idle:
    case state::running:
    case state::completed:
    case state::aborted:
    case state::disposed:
      out = result;
      return true;
  }
}

} // namespace caf::flow::op


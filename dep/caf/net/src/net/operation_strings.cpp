#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/net/operation.hpp"

#include <string>
#include <string_view>

namespace caf::net {

std::string to_string(operation x) {
  switch (x) {
    default:
      return "???";
    case operation::none:
      return "caf::net::operation::none";
    case operation::read:
      return "caf::net::operation::read";
    case operation::write:
      return "caf::net::operation::write";
    case operation::block_read:
      return "caf::net::operation::block_read";
    case operation::block_write:
      return "caf::net::operation::block_write";
    case operation::read_write:
      return "caf::net::operation::read_write";
    case operation::read_only:
      return "caf::net::operation::read_only";
    case operation::write_only:
      return "caf::net::operation::write_only";
    case operation::shutdown:
      return "caf::net::operation::shutdown";
  }
}

bool from_string(std::string_view in, operation& out) {
  if (in == "caf::net::operation::none") {
    out = operation::none;
    return true;
  }
  if (in == "caf::net::operation::read") {
    out = operation::read;
    return true;
  }
  if (in == "caf::net::operation::write") {
    out = operation::write;
    return true;
  }
  if (in == "caf::net::operation::block_read") {
    out = operation::block_read;
    return true;
  }
  if (in == "caf::net::operation::block_write") {
    out = operation::block_write;
    return true;
  }
  if (in == "caf::net::operation::read_write") {
    out = operation::read_write;
    return true;
  }
  if (in == "caf::net::operation::read_only") {
    out = operation::read_only;
    return true;
  }
  if (in == "caf::net::operation::write_only") {
    out = operation::write_only;
    return true;
  }
  if (in == "caf::net::operation::shutdown") {
    out = operation::shutdown;
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
    case operation::none:
    case operation::read:
    case operation::write:
    case operation::block_read:
    case operation::block_write:
    case operation::read_write:
    case operation::read_only:
    case operation::write_only:
    case operation::shutdown:
      out = result;
      return true;
  }
}

} // namespace caf::net


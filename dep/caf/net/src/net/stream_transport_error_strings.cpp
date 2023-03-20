#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/net/stream_transport_error.hpp"

#include <string>
#include <string_view>

namespace caf::net {

std::string to_string(stream_transport_error x) {
  switch (x) {
    default:
      return "???";
    case stream_transport_error::temporary:
      return "caf::net::stream_transport_error::temporary";
    case stream_transport_error::want_read:
      return "caf::net::stream_transport_error::want_read";
    case stream_transport_error::want_write:
      return "caf::net::stream_transport_error::want_write";
    case stream_transport_error::permanent:
      return "caf::net::stream_transport_error::permanent";
  }
}

bool from_string(std::string_view in, stream_transport_error& out) {
  if (in == "caf::net::stream_transport_error::temporary") {
    out = stream_transport_error::temporary;
    return true;
  }
  if (in == "caf::net::stream_transport_error::want_read") {
    out = stream_transport_error::want_read;
    return true;
  }
  if (in == "caf::net::stream_transport_error::want_write") {
    out = stream_transport_error::want_write;
    return true;
  }
  if (in == "caf::net::stream_transport_error::permanent") {
    out = stream_transport_error::permanent;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<stream_transport_error> in,
                  stream_transport_error& out) {
  auto result = static_cast<stream_transport_error>(in);
  switch (result) {
    default:
      return false;
    case stream_transport_error::temporary:
    case stream_transport_error::want_read:
    case stream_transport_error::want_write:
    case stream_transport_error::permanent:
      out = result;
      return true;
  }
}

} // namespace caf::net


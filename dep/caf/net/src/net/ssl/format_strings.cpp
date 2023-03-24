#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/net/ssl/format.hpp"

#include <string>
#include <string_view>

namespace caf::net::ssl {

std::string to_string(format x) {
  switch (x) {
    default:
      return "???";
    case format::pem:
      return "caf::net::ssl::format::pem";
    case format::asn1:
      return "caf::net::ssl::format::asn1";
  }
}

bool from_string(std::string_view in, format& out) {
  if (in == "caf::net::ssl::format::pem") {
    out = format::pem;
    return true;
  }
  if (in == "caf::net::ssl::format::asn1") {
    out = format::asn1;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<format> in,
                  format& out) {
  auto result = static_cast<format>(in);
  switch (result) {
    default:
      return false;
    case format::pem:
    case format::asn1:
      out = result;
      return true;
  }
}

} // namespace caf::net::ssl


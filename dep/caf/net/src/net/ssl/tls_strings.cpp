#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/net/ssl/tls.hpp"

#include <string>
#include <string_view>

namespace caf::net::ssl {

std::string to_string(tls x) {
  switch (x) {
    default:
      return "???";
    case tls::any:
      return "caf::net::ssl::tls::any";
    case tls::v1_0:
      return "caf::net::ssl::tls::v1_0";
    case tls::v1_1:
      return "caf::net::ssl::tls::v1_1";
    case tls::v1_2:
      return "caf::net::ssl::tls::v1_2";
    case tls::v1_3:
      return "caf::net::ssl::tls::v1_3";
  }
}

bool from_string(std::string_view in, tls& out) {
  if (in == "caf::net::ssl::tls::any") {
    out = tls::any;
    return true;
  }
  if (in == "caf::net::ssl::tls::v1_0") {
    out = tls::v1_0;
    return true;
  }
  if (in == "caf::net::ssl::tls::v1_1") {
    out = tls::v1_1;
    return true;
  }
  if (in == "caf::net::ssl::tls::v1_2") {
    out = tls::v1_2;
    return true;
  }
  if (in == "caf::net::ssl::tls::v1_3") {
    out = tls::v1_3;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<tls> in,
                  tls& out) {
  auto result = static_cast<tls>(in);
  switch (result) {
    default:
      return false;
    case tls::any:
    case tls::v1_0:
    case tls::v1_1:
    case tls::v1_2:
    case tls::v1_3:
      out = result;
      return true;
  }
}

} // namespace caf::net::ssl


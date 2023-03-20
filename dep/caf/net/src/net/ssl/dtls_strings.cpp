#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/net/ssl/dtls.hpp"

#include <string>
#include <string_view>

namespace caf::net::ssl {

std::string to_string(dtls x) {
  switch (x) {
    default:
      return "???";
    case dtls::any:
      return "caf::net::ssl::dtls::any";
    case dtls::v1_0:
      return "caf::net::ssl::dtls::v1_0";
    case dtls::v1_2:
      return "caf::net::ssl::dtls::v1_2";
  }
}

bool from_string(std::string_view in, dtls& out) {
  if (in == "caf::net::ssl::dtls::any") {
    out = dtls::any;
    return true;
  }
  if (in == "caf::net::ssl::dtls::v1_0") {
    out = dtls::v1_0;
    return true;
  }
  if (in == "caf::net::ssl::dtls::v1_2") {
    out = dtls::v1_2;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<dtls> in,
                  dtls& out) {
  auto result = static_cast<dtls>(in);
  switch (result) {
    default:
      return false;
    case dtls::any:
    case dtls::v1_0:
    case dtls::v1_2:
      out = result;
      return true;
  }
}

} // namespace caf::net::ssl


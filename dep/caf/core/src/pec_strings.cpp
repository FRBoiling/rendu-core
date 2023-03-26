#include "caf/config.hpp"

CAF_PUSH_DEPRECATED_WARNING

#include "caf/pec.hpp"

#include <string>
#include <string_view>

namespace caf {

std::string to_string(pec x) {
  switch (x) {
    default:
      return "???";
    case pec::success:
      return "caf::pec::success";
    case pec::trailing_character:
      return "caf::pec::trailing_character";
    case pec::unexpected_eof:
      return "caf::pec::unexpected_eof";
    case pec::unexpected_character:
      return "caf::pec::unexpected_character";
    case pec::timespan_overflow:
      return "caf::pec::timespan_overflow";
    case pec::fractional_timespan:
      return "caf::pec::fractional_timespan";
    case pec::too_many_characters:
      return "caf::pec::too_many_characters";
    case pec::invalid_escape_sequence:
      return "caf::pec::invalid_escape_sequence";
    case pec::unexpected_newline:
      return "caf::pec::unexpected_newline";
    case pec::integer_overflow:
      return "caf::pec::integer_overflow";
    case pec::integer_underflow:
      return "caf::pec::integer_underflow";
    case pec::exponent_underflow:
      return "caf::pec::exponent_underflow";
    case pec::exponent_overflow:
      return "caf::pec::exponent_overflow";
    case pec::type_mismatch:
      return "caf::pec::type_mismatch";
    case pec::not_an_option:
      return "caf::pec::not_an_option";
    case pec::invalid_argument:
      return "caf::pec::invalid_argument";
    case pec::missing_argument:
      return "caf::pec::missing_argument";
    case pec::invalid_category:
      return "caf::pec::invalid_category";
    case pec::invalid_field_name:
      return "caf::pec::invalid_field_name";
    case pec::repeated_field_name:
      return "caf::pec::repeated_field_name";
    case pec::missing_field:
      return "caf::pec::missing_field";
    case pec::invalid_range_expression:
      return "caf::pec::invalid_range_expression";
    case pec::invalid_state:
      return "caf::pec::invalid_state";
    case pec::nested_too_deeply:
      return "caf::pec::nested_too_deeply";
  }
}

bool from_string(std::string_view in, pec& out) {
  if (in == "caf::pec::success") {
    out = pec::success;
    return true;
  }
  if (in == "caf::pec::trailing_character") {
    out = pec::trailing_character;
    return true;
  }
  if (in == "caf::pec::unexpected_eof") {
    out = pec::unexpected_eof;
    return true;
  }
  if (in == "caf::pec::unexpected_character") {
    out = pec::unexpected_character;
    return true;
  }
  if (in == "caf::pec::timespan_overflow") {
    out = pec::timespan_overflow;
    return true;
  }
  if (in == "caf::pec::fractional_timespan") {
    out = pec::fractional_timespan;
    return true;
  }
  if (in == "caf::pec::too_many_characters") {
    out = pec::too_many_characters;
    return true;
  }
  if (in == "caf::pec::invalid_escape_sequence") {
    out = pec::invalid_escape_sequence;
    return true;
  }
  if (in == "caf::pec::unexpected_newline") {
    out = pec::unexpected_newline;
    return true;
  }
  if (in == "caf::pec::integer_overflow") {
    out = pec::integer_overflow;
    return true;
  }
  if (in == "caf::pec::integer_underflow") {
    out = pec::integer_underflow;
    return true;
  }
  if (in == "caf::pec::exponent_underflow") {
    out = pec::exponent_underflow;
    return true;
  }
  if (in == "caf::pec::exponent_overflow") {
    out = pec::exponent_overflow;
    return true;
  }
  if (in == "caf::pec::type_mismatch") {
    out = pec::type_mismatch;
    return true;
  }
  if (in == "caf::pec::not_an_option") {
    out = pec::not_an_option;
    return true;
  }
  if (in == "caf::pec::invalid_argument") {
    out = pec::invalid_argument;
    return true;
  }
  if (in == "caf::pec::missing_argument") {
    out = pec::missing_argument;
    return true;
  }
  if (in == "caf::pec::invalid_category") {
    out = pec::invalid_category;
    return true;
  }
  if (in == "caf::pec::invalid_field_name") {
    out = pec::invalid_field_name;
    return true;
  }
  if (in == "caf::pec::repeated_field_name") {
    out = pec::repeated_field_name;
    return true;
  }
  if (in == "caf::pec::missing_field") {
    out = pec::missing_field;
    return true;
  }
  if (in == "caf::pec::invalid_range_expression") {
    out = pec::invalid_range_expression;
    return true;
  }
  if (in == "caf::pec::invalid_state") {
    out = pec::invalid_state;
    return true;
  }
  if (in == "caf::pec::nested_too_deeply") {
    out = pec::nested_too_deeply;
    return true;
  }
  return false;
}

bool from_integer(std::underlying_type_t<pec> in,
                  pec& out) {
  auto result = static_cast<pec>(in);
  switch (result) {
    default:
      return false;
    case pec::success:
    case pec::trailing_character:
    case pec::unexpected_eof:
    case pec::unexpected_character:
    case pec::timespan_overflow:
    case pec::fractional_timespan:
    case pec::too_many_characters:
    case pec::invalid_escape_sequence:
    case pec::unexpected_newline:
    case pec::integer_overflow:
    case pec::integer_underflow:
    case pec::exponent_underflow:
    case pec::exponent_overflow:
    case pec::type_mismatch:
    case pec::not_an_option:
    case pec::invalid_argument:
    case pec::missing_argument:
    case pec::invalid_category:
    case pec::invalid_field_name:
    case pec::repeated_field_name:
    case pec::missing_field:
    case pec::invalid_range_expression:
    case pec::invalid_state:
    case pec::nested_too_deeply:
      out = result;
      return true;
  }
}

} // namespace caf

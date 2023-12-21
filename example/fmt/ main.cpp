
#include <fmt/format.h>
#include <iostream>
template<typename... Args>
void log_fmt(fmt::string_view fmt, Args &&...args) {
  try {
    fmt::basic_memory_buffer<char, 250> buf;
    // seems that fmt::detail::vformat_to(buf, ...) is ~20ns faster than fmt::vformat_to(std::back_inserter(buf),..)
    fmt::detail::vformat_to(buf, fmt, fmt::make_format_args(std::forward<Args>(args)...));
    auto content = fmt::string_view(buf.data(), buf.size());
  } catch (...) {

  }
}

template<typename... Args>
void log_(fmt::string_view fmt, Args &&...args) {
  try {
    fmt::basic_memory_buffer<char, 250> buf;
    // seems that fmt::detail::vformat_to(buf, ...) is ~20ns faster than fmt::vformat_to(std::back_inserter(buf),..)
    fmt::detail::vformat_to(buf, fmt, fmt::make_format_args(std::forward<Args>(args)...));
    auto content = fmt::string_view(buf.data(), buf.size());
  } catch (...) {

  }
}


int main() {
    log_("{}={}={}",1,2,3);
}

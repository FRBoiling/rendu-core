/*
* Created by boil on 2022/9/12.
*/
#include "example_spdlog.h"
#include "example_custom.h"
#include "log.h"
#include "log_trace.h"
#include "spdlog/fmt/fmt.h"

template <typename... T>
std::string StringFormat(fmt::format_string<T...> fmt_str, T&&... args) {
  try
  {
    return vformat(fmt_str, fmt::make_format_args(args...));
  }
  catch (const fmt::format_error& formatError)
  {
//    std::string error = "An error occurred formatting string \"" + std::string(fmt_str) + "\" : " + std::string
//        (formatError.what());
    return "";
  }
}

int main(int, char *[]) {

  std::string message = StringFormat("The answer is {}.{}", 42,"1");
  printf("%s", message.c_str());
  //  spdlog_example();
//  custom_example();
//  spdlog::set_pattern("%+");
  RD_INIT("custom",spdlog::level::trace);
  TLOG("---------{}---------",spdlog::level::debug);
  RD_TRACE("---------1---------");
  RD_DEBUG("---------2---------");
  RD_WARN("---------3---------");
  RD_INFO("---------4---------");
  RD_ERROR("---------5---------");
  RD_CRITICAL("---------6---------");
  TLOG("---------tlog---2------");
}



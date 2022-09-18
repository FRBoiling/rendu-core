/*
* Created by boil on 2022/9/12.
*/
#include "example_spdlog.h"
#include "example_custom.h"
#include "log.h"

int main(int, char *[]) {
//  spdlog_example();
//  custom_example();
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][%t][%l][%L][%^%L%$][%s:%!:%#][%@]%v");
  SPDLOG_INFO("---------1---------");
  spdlog::set_pattern("%+");
  SPDLOG_INFO("---------2---------");

  sLog->trace("-------trace--------");
  sLog->debug("-------debug--------");
  sLog->info("-------info--------");
  sLog->warn("-------warn--------");
  sLog->error("-------error--------");
  sLog->critical("-------critical--------");
}



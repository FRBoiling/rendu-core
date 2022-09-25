/*
* Created by boil on 2022/9/24.
*/

#pragma once
#include "log.h"
#include "example_enum.h"

void log_example(){
  RD_INIT("custom",spdlog::level::trace,"logs/");
  RD_TRACE("------{1}-{0}-{0}------", enum_name(ClassEnum::One),enum_name(ClassEnum::Two),enum_name(ClassEnum::Three));

  RD_TRACE("---------1---------");
  RD_DEBUG("---------2---------");
  RD_WARN("---------3---------");
  RD_INFO("---------4---------");
  RD_ERROR("---------5---------");
  RD_CRITICAL("---------6---------");
}


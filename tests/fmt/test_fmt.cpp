/*
* Created by boil on 2022/10/3.
*/

#include "rd_catch.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bundled/printf.h"

enum class Color{
  Red,
  Blue,
  Green,
};

TEST_CASE("test_fmt") {

  auto num_str = fmt::format("{}_{}_{}_{}", 1,2,3,4);


  auto mix_str = fmt::format("{}:{}_{}_{}", ">>",1,2,3);
  REQUIRE(num_str == "");

//  set1.erase(2);
//
//  REQUIRE(set1.size() == 1);


}
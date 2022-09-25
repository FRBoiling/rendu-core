/*
* Created by boil on 2022/9/12.
*/
#include "example_log.h"
#include "example_string.h"
#include "example_enum.h"

int main(int argc, char **argv) {
  try {
    log_example();
    enum_example();
    RD_INFO("hello world");
  }
  catch (...){
    RD_CRITICAL("catch error");
  }

}



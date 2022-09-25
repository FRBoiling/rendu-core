/*
* Created by boil on 2022/9/12.
*/
#include "example_log.h"
#include "example_args.h"
#include "example_enum.h"

int main(int argc, char **argv) {
  try {
    args_example1(argc,argv);
    log_example();
    enum_example();
    RD_INFO("hello world");
  }
  catch (...){
    RD_CRITICAL("catch error");
  }

}



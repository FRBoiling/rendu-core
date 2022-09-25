/*
* Created by boil on 2022/9/12.
*/
#include "example_log.h"
#include "example_args.h"
#include "example_enum.h"

int main(int argc, char **argv) {
  try {
    example_args(argc,argv);
    example_log();
    example_enum();
    RD_INFO("hello world");
  }
  catch (...){
    RD_CRITICAL("catch error");
  }

}



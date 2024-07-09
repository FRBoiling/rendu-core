/*
* Created by boil on 2023/11/20.
*/

#ifndef RENDU_RDTask_EXAMPLE_H
#define RENDU_RDTask_EXAMPLE_H

#include <spdlog/spdlog.h>
#include "basic/basic_include.h"

using namespace rendu;


void compute(){
  RD_THROW_EXCEPTION("11111")
}


void basic_example(){
  SPDLOG_ERROR("async_example begin...");
  try {
    compute();
  }catch (Exception& ex){
    SPDLOG_ERROR("Exception {}",ex.what());
  }
  SPDLOG_ERROR("所有线程执行完毕。");

  SPDLOG_ERROR("async_example end.");
}



#endif//RENDU_RDTask_EXAMPLE_H

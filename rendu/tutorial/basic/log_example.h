/*
* Created by boil on 2024/8/22.
*/

#ifndef RENDU_TUTORIAL_BASIC_LOG_EXAMPLE_H_
#define RENDU_TUTORIAL_BASIC_LOG_EXAMPLE_H_

#include "basic/log/log_msg.h"
#include "basic/log/logger_wrapper.h"
using namespace rendu;

void log_example() {
    LogMsg log_msg;
    log_msg.setMsg("log msg {}","test11111");
    printf("%s\n", log_msg.getMsg().c_str());
    RD_LOGGER_INIT();
    RD_ERROR("log wrapper {}","test22222")
}

#endif//RENDU_TUTORIAL_BASIC_LOG_EXAMPLE_H_

//
// Created by 沸腾 on 2025/10/24.
//

#ifndef RENDU_TEST_LOG_H
#define RENDU_TEST_LOG_H

#include "common/log/log.h"

void test_log()
{
    std::string logger;
    RC_LOG_INFO(logger,"test log debug");
}

#endif //RENDU_TEST_LOG_H
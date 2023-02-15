#**********************************
#  Created by boil on 2022/8/14.
#**********************************

rendu_add_test(
    NAME
    string
    CMAKE_CUR_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    CMAKE_CUR_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_BINARY_DIR
    ${CMAKE_BINARY_DIR}
    DEPS
    rendu::common
    gtest::gmock
    gtest::gtest
    DEFINES
    LINKOPTS
)

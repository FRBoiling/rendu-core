#**********************************
#  Created by boil on 2022/8/14.
#**********************************

rendu_add_test(
    NAME
    rendu
    CMAKE_CUR_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    CMAKE_CUR_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_BINARY_DIR
    ${CMAKE_BINARY_DIR}
    DEPS
    rendu::test
    rendu::core
    rendu::common
    DEFINES
    LINKOPTS
)

#rendu_cc_test(
#    NAME
#    meta_type
#    SRCS
#    ${CMAKE_CURRENT_SOURCE_DIR}/core/test_meta_type.cpp
#    COPTS
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#)

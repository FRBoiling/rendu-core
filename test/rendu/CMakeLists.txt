#**********************************
#  Created by boil on 2022/8/14.
#**********************************

rendu_batch_test(
    NAME
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

#rendu_add_test(
#    NAME
#    rendu
#    CMAKE_CUR_SOURCE_DIR
#    ${CMAKE_CURRENT_SOURCE_DIR}/core/meta
#    CMAKE_CUR_BINARY_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}/core/meta
#    CMAKE_BINARY_DIR
#    ${CMAKE_BINARY_DIR}/core/meta
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#    DEFINES
#    LINKOPTS
#)

#rendu_cc_test(
#    NAME
#    rendu
#    SRCS
#    ${CMAKE_CURRENT_SOURCE_DIR}/core/meta/test_meta_type.cpp
#    COPTS
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#)

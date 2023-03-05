#**********************************
#  Created by boil on 2022/8/14.
#**********************************

rendu_add_test(
    NAME
    protobuf
    CMAKE_CUR_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    CMAKE_CUR_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_BINARY_DIR
    ${CMAKE_BINARY_DIR}
    DEPS
    rendu::test
    protobuf::libprotoc
    DEFINES
    LINKOPTS
)

#rendu_add_test(
#    NAME
#    rendu_base
#    CMAKE_CUR_SOURCE_DIR
#    ${CMAKE_CURRENT_SOURCE_DIR}/core/base
#    CMAKE_CUR_BINARY_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}/core/base
#    CMAKE_BINARY_DIR
#    ${CMAKE_BINARY_DIR}/core/base
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#    DEFINES
#    LINKOPTS
#)
#
#rendu_add_test(
#    NAME
#    rendu_container
#    CMAKE_CUR_SOURCE_DIR
#    ${CMAKE_CURRENT_SOURCE_DIR}/core/container
#    CMAKE_CUR_BINARY_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}/core/container
#    CMAKE_BINARY_DIR
#    ${CMAKE_BINARY_DIR}/core/container
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#    DEFINES
#    LINKOPTS
#)
#
#rendu_add_test(
#    NAME
#    rendu_define
#    CMAKE_CUR_SOURCE_DIR
#    ${CMAKE_CURRENT_SOURCE_DIR}/core/define
#    CMAKE_CUR_BINARY_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}/core/define
#    CMAKE_BINARY_DIR
#    ${CMAKE_BINARY_DIR}/core/define
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#    DEFINES
#    LINKOPTS
#)
#
#rendu_add_test(
#    NAME
#    rendu_ecs
#    CMAKE_CUR_SOURCE_DIR
#    ${CMAKE_CURRENT_SOURCE_DIR}/core/ecs
#    CMAKE_CUR_BINARY_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}/core/ecs
#    CMAKE_BINARY_DIR
#    ${CMAKE_BINARY_DIR}/core/ecs
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#    DEFINES
#    LINKOPTS
#)
#
#rendu_add_test(
#    NAME
#    rendu_locator
#    CMAKE_CUR_SOURCE_DIR
#    ${CMAKE_CURRENT_SOURCE_DIR}/core/locator
#    CMAKE_CUR_BINARY_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}/core/locator
#    CMAKE_BINARY_DIR
#    ${CMAKE_BINARY_DIR}/core/locator
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#    DEFINES
#    LINKOPTS
#)
#
#rendu_add_test(
#    NAME
#    rendu_signal
#    CMAKE_CUR_SOURCE_DIR
#    ${CMAKE_CURRENT_SOURCE_DIR}/core/signal
#    CMAKE_CUR_BINARY_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}/core/signal
#    CMAKE_BINARY_DIR
#    ${CMAKE_BINARY_DIR}/core/locator
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#    DEFINES
#    LINKOPTS
#)
#
#rendu_add_test(
#    NAME
#    rendu_meta
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

#rendu_batch_test(
#    NAME
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
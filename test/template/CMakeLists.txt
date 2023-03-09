#**********************************
#  Created by boil on 2022/8/14.
#**********************************

#rendu_batch_test(
#    NAME
#    CMAKE_CUR_SOURCE_DIR
#    ${CMAKE_CURRENT_SOURCE_DIR}
#    CMAKE_CUR_BINARY_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}
#    CMAKE_BINARY_DIR
#    ${CMAKE_BINARY_DIR}
#    DEPS
#    rendu::test
#    rendu::core
#    rendu::common
#    DEFINES
#    LINKOPTS
#)

rendu_add_executable(
    NAME
    template
    SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    rendu-dependency-interface
    DEPS
    rendu::test
    DEFINES
    LINKOPTS
)


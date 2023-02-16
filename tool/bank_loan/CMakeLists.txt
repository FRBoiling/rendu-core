#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name bank_loan)
rendu_add_executable(
    NAME
    ${target_name}
    CMAKE_CUR_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    CMAKE_CUR_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_BINARY_DIR
    ${CMAKE_BINARY_DIR}
    PRIVATE_DEPS
    rendu-core-interface
    PUBLIC_DEPS
    DEFINES
    LINKOPTS
)
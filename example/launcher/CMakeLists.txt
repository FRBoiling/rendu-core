#**********************************
#  Created by boil on 2022/8/14.
#**********************************

rendu_add_executable(
    NAME
    launcher
    CMAKE_CUR_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    CMAKE_CUR_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_BINARY_DIR
    ${CMAKE_BINARY_DIR}
    PRIVATE_DEPS
    rendu-core-interface
    PUBLIC_DEPS
    rendu::common
    rendu::core
    DEFINES
    LINKOPTS
)




#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(PROJECT_NAME rendu)
rendu_add_library(
    NAME
    test
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


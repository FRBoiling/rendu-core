#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(PROJECT_NAME zlib)
rendu_add_library(
    NAME
    zlib
    CMAKE_CUR_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    CMAKE_CUR_BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_BINARY_DIR
    ${CMAKE_BINARY_DIR}
    DEPS
    DEFINES
    LINKOPTS
)


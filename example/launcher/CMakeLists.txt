#**********************************
#  Created by boil on 2022/8/14.
#**********************************

rendu_add_executable(
    NAME
    launcher
    SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    rendu-core-interface
    DEPS
    rendu::core
    DEFINES
    LINKOPTS
)




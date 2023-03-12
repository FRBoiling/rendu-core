#**********************************
#  Created by boil on 2022/8/14.
#**********************************
set(target_name launcher)
rendu_add_executable(
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    rendu-dependency-interface
    rendu-core-interface
    DEPS
    rendu::core
    rendu::common
    DEFINES
    LINKOPTS
)
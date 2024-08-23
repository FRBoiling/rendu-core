#**********************************
#  Created by boil on 2022/8/14.
#**********************************
get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

rendu_add_executable(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    LINKOPTS
    rendu-core-interface
    DEPS
    rendu::core
    rendu::loader
    DEFINES
    -DSPDLOG_ACTIVE_LEVEL=0
)


rendu_add_library(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${FOLDER_NAME}
    LINKOPTS
    rendu-core-interface
    DEPS
    rendu::loader
    DEFINES
    -DSPDLOG_ACTIVE_LEVEL=0
)

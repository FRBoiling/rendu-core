#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core rendu module
#|--------------------------------

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
rendu_add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
    #    ${CMAKE_CURRENT_SOURCE_DIR}/basic
    #    ${CMAKE_CURRENT_SOURCE_DIR}/io
    #    ${CMAKE_CURRENT_SOURCE_DIR}/time

    #    ${CMAKE_CURRENT_SOURCE_DIR}/async
    #    ${CMAKE_CURRENT_SOURCE_DIR}/timer

    #    ${CMAKE_CURRENT_SOURCE_DIR}/tutorial

    ${CMAKE_CURRENT_SOURCE_DIR}/app
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/ecs
    ${CMAKE_CURRENT_SOURCE_DIR}/loader
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/net
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime
    ${CMAKE_CURRENT_SOURCE_DIR}/serialize
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
    ${CMAKE_CURRENT_SOURCE_DIR}/task
)

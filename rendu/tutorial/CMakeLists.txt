#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu common example projects
#|--------------------------------

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(PROJECT_NAME ${PROJECT_NAME}_${FOLDER_NAME})

rendu_add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
    ${CMAKE_CURRENT_SOURCE_DIR}/base
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/net
    ${CMAKE_CURRENT_SOURCE_DIR}/task
    ${CMAKE_CURRENT_SOURCE_DIR}/test
    ${CMAKE_CURRENT_SOURCE_DIR}/thread
    ${CMAKE_CURRENT_SOURCE_DIR}/time
)

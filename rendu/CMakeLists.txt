#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core rendu module
#|--------------------------------

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "正在配置 ${PROJECT_NAME} ")

rendu_add_subdirectories(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        EXCLUDE_DIRS
)

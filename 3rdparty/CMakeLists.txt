#**********************************
#  Created by boil on 2022/8/14.
#**********************************

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
rendu_add_subdirectories(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        EXCLUDE_DIRS
)
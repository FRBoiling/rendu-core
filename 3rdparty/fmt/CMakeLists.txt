#**********************************
#  Created by boil on 2026/1/9.
#**********************************

rendu_find_package(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 12.1.0
)

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

rendu_add_library(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME ${FOLDER_NAME}
        PRIVATE_LINK rendu-dependency-interface
        PUBLIC_LINK fmt::fmt
        DEFINES
        INTERFACE
        ALIAS ON
)
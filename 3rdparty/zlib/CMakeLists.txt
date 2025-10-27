#**********************************
#  Created by boil on 2023/4/9.
#**********************************
get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)


rendu_find_package(
        NAME Spdlog
        GITHUB_REPOSITORY madler/zlib
        GIT_TAG v1.3.1
)

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

rendu_add_library(
        DIR     ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT    ${PROJECT_NAME}
        NAME    ${FOLDER_NAME}
        PRIVATE_LINK    rendu-dependency-interface
        PUBLIC_LINK Spdlog::Spdlog
        DEFINES
        INTERFACE
        ALIAS     ON
)


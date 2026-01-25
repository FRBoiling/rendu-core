#**********************************
#  Created by boil on 2026/1/25.
#**********************************

rendu_find_package(
        NAME simdjson
        GITHUB_REPOSITORY simdjson/simdjson
        GIT_TAG v3.9.4
        EXCLUDE_FROM_ALL YES
        SYSTEM YES
        OPTIONS
        "SIMDJSON_BUILD_STATIC ON"
        "SIMDJSON_ENABLE_THREADS ON"
        "SIMDJSON_BUILD_TESTS OFF"
        "SIMDJSON_BUILD_EXAMPLES OFF"
        "SIMDJSON_COMPETITION OFF"
        "SIMDJSON_FUZZING OFF"
        "SIMDJSON_INSTALL OFF"
)

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

rendu_add_library(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME ${FOLDER_NAME}
        PRIVATE_LINK rendu-dependency-interface
        PUBLIC_LINK simdjson::simdjson
        DEFINES
        INTERFACE
        ALIAS ON
)

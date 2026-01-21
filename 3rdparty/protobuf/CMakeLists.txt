#**********************************
#  Created by boil on 2026/1/21.
#**********************************

rendu_find_package(
        NAME Protobuf
        GITHUB_REPOSITORY protocolbuffers/protobuf
        GIT_TAG v29.1
        CMAKE_ARGS -Dprotobuf_BUILD_TESTS=OFF
)

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

rendu_add_library(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME ${FOLDER_NAME}
        PRIVATE_LINK rendu-dependency-interface
        PUBLIC_LINK protobuf::libprotobuf
        DEFINES
        INTERFACE
        ALIAS ON
)

#**********************************
#  Created by boil on 2023/4/9.
#**********************************

rendu_find_package(
        NAME Protobuf
        GITHUB_REPOSITORY protocolbuffers/protobuf
        GIT_TAG v33.0
)

#rendu_find_package(
#        NAME protobuf
#        VERSION 3.21.12
#        URL https://github.com/protocolbuffers/protobuf/releases/download/v21.12/protobuf-cpp-3.21.12.tar.gz
#        URL_HASH SHA256=4eab9b524aa59191c7b736d4a5fe8a241c0c21bd7d52c3d38a7d276f82e70d01
#        OPTIONS
#        -Dprotobuf_BUILD_TESTS=OFF
#)



get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

rendu_add_library(
        DIR     ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT    ${PROJECT_NAME}
        NAME    ${FOLDER_NAME}
        PRIVATE_LINK    rendu-dependency-interface
        PUBLIC_LINK protobuf::protobuf
        DEFINES
        INTERFACE
        ALIAS     ON
)

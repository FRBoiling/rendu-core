#**********************************
#  Created by boil on 2026/1/21.
#**********************************

rendu_find_package(
        NAME Protobuf
        GITHUB_REPOSITORY protocolbuffers/protobuf
        GIT_TAG v33.4
#        CMAKE_ARGS
#        -Dprotobuf_BUILD_TESTS=OFF
#        -Dprotobuf_BUILD_EXAMPLES=OFF
#        -Dprotobuf_BUILD_SHARED_LIBS=OFF
#        -Dprotobuf_ABSL_PROVIDER=package
#        -DCMAKE_POLICY_DEFAULT_CMP0141=NEW
#        -DABSL_PROPAGATE_CXX_STD=ON
#        -Dprotobuf_BUILD_CONFORMANCE=OFF
#        -Dprotobuf_BUILD_LIBPROTOC=ON
#        -DCMAKE_CXX_FLAGS=
#        -DCMAKE_C_FLAGS=
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

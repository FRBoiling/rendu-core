#**********************************
#  Created by boil on 2026/1/21.
#**********************************

rendu_find_package(
        NAME Protobuf
        GITHUB_REPOSITORY protocolbuffers/protobuf
        GIT_TAG v33.4
        EXCLUDE_FROM_ALL YES
        SYSTEM YES
        OPTIONS
        "protobuf_BUILD_TESTS OFF"
        "protobuf_BUILD_EXAMPLES OFF"
        "protobuf_BUILD_SHARED_LIBS OFF"
        "protobuf_ABSL_PROVIDER package"
        "CMAKE_POLICY_DEFAULT_CMP0141 NEW"
        "ABSL_PROPAGATE_CXX_STD ON"
        "protobuf_BUILD_CONFORMANCE OFF"
        "protobuf_BUILD_LIBPROTOC ON"
        "utf8_range_ENABLE_INSTALL OFF"
        "protobuf_INSTALL OFF"
        "CMAKE_CXX_FLAGS "
        "CMAKE_C_FLAGS "
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

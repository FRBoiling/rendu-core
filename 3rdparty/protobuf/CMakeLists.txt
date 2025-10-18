#**********************************
#  Created by boil on 2023/4/9.
#**********************************

#**********************************
#  Created by boil on 2023/4/9.
#**********************************

# 在包含 protobuf 之前设置选项
set(protobuf_BUILD_TESTS OFF)
set(protobuf_BUILD_EXAMPLES OFF)

# 原有的Protobuf查找配置
rendu_find_package(
        NAME Protobuf
        GITHUB_REPOSITORY protocolbuffers/protobuf
        GIT_TAG v33.0
)

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

rendu_add_library(
        DIR     ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT    ${PROJECT_NAME}
        NAME    ${FOLDER_NAME}
        LINKOPTS    rendu-dependency-interface
        DEPS protobuf::protobuf
        DEFINES
        INTERFACE
        ALIAS     ON
)

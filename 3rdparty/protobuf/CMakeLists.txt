#**********************************
#  Created by boil on 2023/4/9.
#**********************************

#**********************************
#  Created by boil on 2023/4/9.
#**********************************

## 添加Protobuf依赖的GTest和Abseil查找配置
#rendu_find_package(
#        NAME GTest
#        GITHUB_REPOSITORY google/googletest
#        VERSION 1.17.0
#)
#
#rendu_find_package(
#        NAME absl
#        GITHUB_REPOSITORY abseil/abseil-cpp
#        VERSION 20250512.1
#)

# 原有的Protobuf查找配置
rendu_find_package(
        NAME Protobuf
        GITHUB_REPOSITORY protocolbuffers/protobuf
        VERSION 32.0
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

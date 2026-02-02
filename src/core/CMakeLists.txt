#**********************************
#  Created by boil on 2022/9/10.
#**********************************

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "正在配置 ${FOLDER_NAME} ")

rendu_add_library(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        ${FOLDER_NAME}
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK
        RenduCore::common
        DEFINES
        RENDU_CORE_EXPORT
        ALIAS ON
)

# 添加 protobuf 生成目录到包含路径
target_include_directories(${PROJECT_NAME}_${FOLDER_NAME} PUBLIC
        ${CMAKE_BINARY_DIR}/src/apps/protocol/proto_gen
)

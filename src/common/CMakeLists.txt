#**********************************
#  Created by boil on 2022/9/10.
#**********************************

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "正在配置 ${FOLDER_NAME} ")
# 先添加这行
include_directories(${RENDU_BUILDDIR})
rendu_add_library(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        ${FOLDER_NAME}
        LINKOPTS
        rendu-dependency-interface
        rendu-core-interface
        DEPS
        dep::entt
        dep::spdlog
        dep::asio
        dep::fmt
        DEFINES
        ALIAS ON
)

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
        3rdparty::fmt
        3rdparty::asio
        3rdparty::entt
        DEFINES
        ALIAS ON
)

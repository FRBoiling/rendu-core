#**********************************
#  Created by boil on 2022/8/14.
#**********************************
# 注意：所有测试已迁移到 src/apps/tests/ 目录
# 此目录仅保留 main.cpp 作为功能演示
#**********************************

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        ${FOLDER_NAME}
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::core
        DEFINES
        ALIAS ON
        SOURCES
        main.cpp
)
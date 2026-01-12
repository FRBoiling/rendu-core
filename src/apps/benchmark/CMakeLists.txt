#**********************************
#  Created by boil on 2026/1/15.
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
        PUBLIC_LINK RenduCore::core 3rdparty::entt
        DEFINES
        INTERFACE
        ALIAS ON
)

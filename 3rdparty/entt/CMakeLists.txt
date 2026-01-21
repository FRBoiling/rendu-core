#**********************************
#  Created by boil on 2026/1/9.
#**********************************

    rendu_find_package(
            NAME entt
            GITHUB_REPOSITORY skypjack/entt
            GIT_TAG v3.16.0
    )

    get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    rendu_add_library(
            DIR ${CMAKE_CURRENT_SOURCE_DIR}
            PROJECT ${PROJECT_NAME}
            NAME ${FOLDER_NAME}
            PRIVATE_LINK rendu-dependency-interface
            PUBLIC_LINK EnTT::EnTT
            DEFINES
            INTERFACE
            ALIAS ON
    )


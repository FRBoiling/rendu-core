#**********************************
#  Created by boil on 2026/1/22.
#**********************************

rendu_find_package(
        NAME absl
        GITHUB_REPOSITORY abseil/abseil-cpp
        GIT_TAG 20250512.1
)

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

rendu_add_library(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME ${FOLDER_NAME}
        PRIVATE_LINK rendu-dependency-interface
        PUBLIC_LINK absl::strings absl::synchronization absl::time absl::status absl::optional absl::variant absl::check absl::core_headers
        DEFINES
        INTERFACE
        ALIAS ON
)

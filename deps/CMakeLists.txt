#**********************************
#  Created by boil on 2022/8/14.
#**********************************
include(FetchContent)
set(RENDU_PROJECT_NAME deps)
rendu_add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
)
message("")

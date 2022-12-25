#**********************************
#  Created by boil on 2022/8/14.
#**********************************
set(project-name deps)
message("====== ${project-name} ======")
include(FetchContent)
rendu_add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
)

message("")

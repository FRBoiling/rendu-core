#**********************************
#  Created by boil on 2023/03/18.
#**********************************

include(RenduProtobufHelper)
set(PROJECT_NAME proto)
rendu_add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
    #      ${CMAKE_CURRENT_SOURCE_DIR}/proto
)

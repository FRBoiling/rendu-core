#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core rendu module
#|--------------------------------
message("")
message(STATUS "Building rendu ...")
AddSubdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
    #    ${CMAKE_CURRENT_SOURCE_DIR}/gtest
    #    ${CMAKE_CURRENT_SOURCE_DIR}/catch2
)

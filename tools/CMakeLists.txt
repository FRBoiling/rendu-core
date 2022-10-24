#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core tools module
#|--------------------------------
message("")
message(STATUS "Building tools ...")
AddSubdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
    #    ${CMAKE_CURRENT_SOURCE_DIR}/proto_generate
)



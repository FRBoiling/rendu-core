#**********************************
#  Created by boil on 2022/8/14.
#**********************************
message("")
message(STATUS "Building deps...")
AddSubdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
#    ${CMAKE_CURRENT_SOURCE_DIR}/protobuf

)

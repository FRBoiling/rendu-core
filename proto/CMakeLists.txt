#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(PROTOBUF_PROTOC_EXECUTABLE proto_generate)
set(PROTO_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

AddSubdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
    ${CMAKE_CURRENT_SOURCE_DIR}/out
)

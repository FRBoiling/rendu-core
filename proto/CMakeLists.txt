#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(PROTOBUF_PROTOC_EXECUTABLE proto_generate)
set(PROTO_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(OUT_PUT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/proto)

AddSubdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
    ${CMAKE_CURRENT_SOURCE_DIR}/test_out
)

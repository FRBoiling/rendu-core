#**********************************
#  Created by boil on 2022/8/14.
#**********************************
rendu_add_executable(
    NAME
    proto_generator
    SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE

    DEPS
    protobuf::libprotobuf
    protobuf::libprotobuf-lite
    protobuf::libprotoc
    DEFINES
    LINKOPTS
)
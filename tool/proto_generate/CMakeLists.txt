#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name proto_generator)
rendu_add_executable(
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    SETTING
    rendu-core-interface

    DEPS
    protobuf::libprotobuf
    protobuf::libprotobuf-lite
    protobuf::libprotoc
    DEFINES
    LINKOPTS
)
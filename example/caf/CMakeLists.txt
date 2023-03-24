#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name caf_ping_pong)
rendu_add_executable(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    SETTING
    rendu-core-interface
    DEPS
    dep::protobuf-lite
    proto::model
    dep::caf_core
    dep::caf_net
    dep::caf_io
    DEFINES
    LINKOPTS
)
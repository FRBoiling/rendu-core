#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name caf_ping)
rendu_add_executable(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    LINKOPTS
    rendu-core-interface
    DEPS
    dep::protobuf-lite
    proto::model
    dep::entt
    dep::caf_io
    example::caf_ping_pong_define
    DEFINES
)
#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name caf_ping_pong)
rendu_add_test(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    SETTING
    rendu-core-interface
    DEPS
    proto::model
    dep::protobuf-lite
    dep::caf_core
    dep::caf_io
    rendu::test
    DEFINES
    LINKOPTS
)

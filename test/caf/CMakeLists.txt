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
    LINKOPTS
    rendu-core-interface
    DEPS
    proto::model
    dep::protobuf
    dep::caf
    dep::caf
    rendu::test
    DEFINES
)

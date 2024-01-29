#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name time)
rendu_add_executable(
    DIR
    .
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    LINKOPTS
    rendu-core-interface
    DEPS
    dep::date
    rendu::base
    rendu::log
    DEFINES
)
#**********************************
#  Created by boil on 2022/8/14.
#**********************************
set(target_name toolkit_echo_server)
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
    dep::toolkit
    DEFINES
)


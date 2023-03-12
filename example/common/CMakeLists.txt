#**********************************
#  Created by boil on 2022/8/14.
#**********************************
set(target_name common)
rendu_add_executable(
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    SETTING
    rendu-core-interface
    DEPS
    DEFINES
    LINKOPTS
)


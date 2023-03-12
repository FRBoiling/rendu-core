#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name string)
rendu_add_executable(
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    SETTING
    rendu-core-interface
    DEPS
    rendu::common
    rendu::test
    DEFINES
    LINKOPTS
)

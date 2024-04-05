#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name snapshot)
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
    third_party::entt
    third_party::cr
    third_party::cereal
    rendu::test
    DEFINES
)

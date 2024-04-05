#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target_name shared)
rendu_add_library(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    LINKOPTS
    rendu-core-interface
    DEPS
    rendu::base
    proto::model
    third_party::entt
    third_party::magic_enum
)

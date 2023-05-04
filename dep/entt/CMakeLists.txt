#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(EnTT CONFIG REQUIRED)
add_library(entt INTERFACE)
target_link_libraries(entt
    INTERFACE
    EnTT::EnTT
    )
add_library(dep::entt ALIAS entt)


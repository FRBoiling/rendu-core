#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(cereal CONFIG REQUIRED)
add_library(cereal INTERFACE)
target_link_libraries(cereal
    INTERFACE
    cereal::cereal
    )
add_library(dep::cereal ALIAS cereal)

#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(cppcoro CONFIG REQUIRED)
add_library(cppcoro1 INTERFACE)
target_link_libraries(cppcoro1
    INTERFACE
    cppcoro
    )
add_library(dep::cppcoro ALIAS cppcoro1)

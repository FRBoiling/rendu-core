#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(fmt CONFIG REQUIRED)
add_library(fmt INTERFACE)
target_link_libraries(fmt
    INTERFACE
    fmt::fmt-header-only
    )
add_library(dep::fmt ALIAS fmt)


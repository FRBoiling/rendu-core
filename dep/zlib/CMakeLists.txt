#**********************************
#  Created by boil on 2022/9/10.
#**********************************
find_package(ZLIB REQUIRED)
add_library(zlib INTERFACE)
target_link_libraries(zlib
    INTERFACE
    ZLIB
    )
add_library(dep::zlib ALIAS zlib)


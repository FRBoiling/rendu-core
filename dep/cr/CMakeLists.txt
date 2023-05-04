#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_path(CR_INCLUDE_DIRS "cr.h")
add_library(cr INTERFACE)
target_include_directories(cr
    INTERFACE
    ${CR_INCLUDE_DIRS}
    )
add_library(dep::cr ALIAS cr)

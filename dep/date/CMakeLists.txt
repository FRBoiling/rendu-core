#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(date REQUIRED)
add_library(date INTERFACE)
target_link_libraries(date
    INTERFACE
    date::date-tz
    )
add_library(dep::date ALIAS date)

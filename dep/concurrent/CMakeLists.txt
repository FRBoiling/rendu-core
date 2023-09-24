#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(unofficial-concurrentqueue CONFIG REQUIRED)

add_library(concurrent INTERFACE)
target_link_libraries(concurrent
    INTERFACE
    unofficial::concurrentqueue::concurrentqueue
    )
add_library(dep::concurrent ALIAS concurrent)


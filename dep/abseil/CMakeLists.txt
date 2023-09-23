#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(absl REQUIRED)
add_library(abseil INTERFACE)
target_link_libraries(abseil
    INTERFACE
    absl::time_zone
    )
add_library(dep::abseil ALIAS abseil)

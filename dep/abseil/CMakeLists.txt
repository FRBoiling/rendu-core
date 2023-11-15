#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(absl CONFIG REQUIRED)
add_library(abseil INTERFACE)
target_link_libraries(abseil
    INTERFACE
    absl::any
    absl::log
    absl::base
    absl::bits
    )
add_library(dep::abseil ALIAS abseil)

#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(spdlog CONFIG REQUIRED)
add_library(spdlog INTERFACE)
target_link_libraries(spdlog
    INTERFACE
    spdlog::spdlog
    spdlog::spdlog_header_only
    )

add_library(dep::spdlog ALIAS spdlog)

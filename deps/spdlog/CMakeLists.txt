#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target-name spdlog)
message(STATUS ">>" ${target-name})
FetchContent_Declare(
    ${target-name}
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.11.0.tar.gz
    FIND_PACKAGE_ARGS NAMES ${target-name}
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
# This will try calling find_package() first for both dependencies
FetchContent_MakeAvailable(${target-name})
message(STATUS  ${target-name}  " Make Success." )


#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target-name gtest)
message(STATUS ">>" ${target-name})
FetchContent_Declare(
    ${target-name}
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
    FIND_PACKAGE_ARGS NAMES ${target-name}
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
# This will try calling find_package() first for both dependencies
FetchContent_MakeAvailable(${target-name})
message(STATUS  ${target-name}  " Make Success." )

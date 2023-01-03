#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target-name magic_enum)
message(STATUS ">>" ${target-name})
FetchContent_Declare(
    ${target-name}
    URL https://github.com/Neargye/magic_enum/archive/refs/tags/v0.8.2.tar.gz
    #    GIT_REPOSITORY https://gitee.com/lovelinpeach/magic_enum.git
    #    GIT_TAG v0.8.2
    #    URL https://gitee.com/lovelinpeach/magic_enum/repository/archive/v0.8.2
    FIND_PACKAGE_ARGS NAMES ${target-name}
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
set(MAGIC_ENUM_OPT_BUILD_TESTS OFF)
set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF)
# This will try calling find_package() first for both dependencies
FetchContent_MakeAvailable(${target-name})
message(STATUS ${target-name} " Make Success.")



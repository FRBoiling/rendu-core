#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target-name abseil)
message(STATUS ">>" ${target-name})

FetchContent_Declare(
    ${target-name}
    URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20220623.1.tar.gz
    #    GIT_REPOSITORY https://gitee.com/lovelinpeach/abseil-cpp.git
    #    GIT_TAG 20220623.1
    #    URL https://gitee.com/lovelinpeach/abseil-cpp/repository/archive/20220623.1
    FIND_PACKAGE_ARGS NAMES ${target-name}
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

# This will try calling find_package() first for both dependencies
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_RANDOM_RANDEN_COPTS_WARNING ON)
set(ABSL_BUILD_DLL ON)

FetchContent_MakeAvailable(${target-name})
message(STATUS ${target-name} " Make Success.")


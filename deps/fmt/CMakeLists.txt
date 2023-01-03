##**********************************
##  Created by boil on 2022/9/10.
##**********************************
#set(target-name fmt)
#message(STATUS ">>" ${target-name})
#FetchContent_Declare(
#    ${target-name}
#    URL https://github.com/fmtlib/fmt/archive/refs/tags/9.1.0.tar.gz
#    #    GIT_REPOSITORY https://gitee.com/lovelinpeach/fmt.git
#    #    GIT_TAG 9.1.0
#    #    URL https://gitee.com/lovelinpeach/fmt/repository/archive/9.1.0
#    FIND_PACKAGE_ARGS NAMES ${target-name}
#    DOWNLOAD_EXTRACT_TIMESTAMP true
#)
## This will try calling find_package() first for both dependencies
#FetchContent_MakeAvailable(${target-name})
#message(STATUS ${target-name} " Make Success.")

#**********************************
#  Created by boil on 2022/9/10.
#**********************************
set(target-name fmt)
set(header_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
list(APPEND RENDU_COMMON_INCLUDE_DIRS ${header_dir})
message(STATUS ">>" ${target-name})
CollectSourceFiles(
    ${sources_dir}
    ${target-name}_sources
    # Exclude

)
CollectHeaderFiles(
    ${header_dir}
    ${target-name}_headers
    # Exclude
)
list(APPEND project_copts
    ${RENDU_DEFAULT_COPTS}
    )
message(------>>>>>>${${target-name}_sources})
rendu_cc_library(${target-name} ${target-name}
    NAME
    ${target-name}
    HDRS
    ${${target-name}_headers}
    SRCS
    ${sources_dir}/format.cc
    ${sources_dir}/os.cc
    COPTS
    ${project_copts}
    DEPS
    DEFINES
    LINKOPTS
    DEPS
    )
message(STATUS  ${target-name}  " Make Success." )





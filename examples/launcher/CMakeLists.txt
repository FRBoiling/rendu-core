#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name launcher)
set(sources ${target_name}_sources)
set(headers ${target_name}_headers)
set(header_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})

CollectSourceFiles(
    ${sources_dir}
    ${sources}
    # Exclude
)
CollectHeaderFiles(
    ${header_dir}
    ${headers}
    # Exclude
)

#rendu_add_executable(
#    NAME
#    ${target_name}
#    HDRS
#    ${${headers}}
#    SRCS
#    ${${sources}}
#    COPTS
#    ${${copts}}
#    DEPS
#    ${${deps}}
#    DEFINES
#    ${${defines}}
#    LINKOPTS
#    ${${linkopts}}
#)


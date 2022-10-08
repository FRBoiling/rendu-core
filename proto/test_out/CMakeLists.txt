#**********************************
#  Created by boil on 2022/9/10.
#**********************************

set(target-name proto_test_out)
set(project-name deps)

CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
    private_sources
    # Exclude
)

add_library(${target-name}  ${private_sources})

target_compile_definitions(${target-name}
    INTERFACE
    )

target_link_libraries(${target-name}
    PRIVATE rendu-dependency-interface
    #    PUBLIC
    PUBLIC libprotobuf
    )
set_target_properties(${target-name} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${target-name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${target-name} PROPERTIES FOLDER ${project-name})

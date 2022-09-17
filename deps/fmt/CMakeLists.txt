#**********************************
#  Created by boil on 2022/9/10.
#**********************************

set(target-name fmt)
set(project-name deps)

CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    private_sources
    # Exclude
)

CollectIncludeDirectories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    public_includes
    # Exclude
)

add_library(${target-name} STATIC ${private_sources} ${public_includes})

#target_compile_definitions(${target-name}
#    PRIVATE
#    -Wdeprecated-declarations
#    )

target_link_libraries(${target-name}
    PRIVATE rendu-dependency-interface
    )

set_target_properties(${target-name} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${target-name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${target-name} PROPERTIES FOLDER ${project-name})

#**********************************
#  Created by boil on 2022/9/10.
#**********************************

set(target-name cmdline)
set(project-name deps)

CollectIncludeDirectories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    public_includes
    # Exclude
)

add_library(${target-name} INTERFACE ${public_includes})

target_link_libraries(${target-name}
    INTERFACE rendu-dependency-interface
    defines
    )

set_target_properties(${target-name} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${target-name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${target-name} PROPERTIES FOLDER ${project-name})
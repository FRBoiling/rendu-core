#**********************************
#  Created by boil on 2022/9/10.
#**********************************

set(target-name defines)
set(project-name deps)

CollectIncludeDirectories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    public_includes
    # Exclude
)

add_library(${target-name} INTERFACE ${public_includes})

target_compile_definitions(${target-name}
    INTERFACE
    #    -DRAPIDJSON_HAS_STDSTRING
    #    -DRAPIDJSON_48BITPOINTER_OPTIMIZATION=0
    #    -DRAPIDJSON_ASSERT=WPAssert
    )

#if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64)|(AMD64|amd64)|(^i.86$)")
#  target_compile_definitions(${target-name} INTERFACE -DRAPIDJSON_SSE2)
#elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
#  target_compile_definitions(${target-name} INTERFACE -DRAPIDJSON_NEON)
#elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
#  target_compile_definitions(${target-name} INTERFACE -DRAPIDJSON_NEON)
#endif ()

set_target_properties(${target-name} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${target-name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${target-name} PROPERTIES FOLDER ${project-name})
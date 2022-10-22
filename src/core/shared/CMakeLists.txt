#**********************************
#  Created by boil on 2022/10/20.
#**********************************
set(target-name shared)
set(project-name core)

CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE_SOURCES
    # Exclude
    ${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers)

if(USE_COREPCH)
  set(PRIVATE_PCH_HEADER precompiled_headers/${target-name}_pch.h)
endif()

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DRENDU_API_EXPORT_SHARED)

add_library(shared
    ${PRIVATE_PCH_SOURCE}
    ${PRIVATE_SOURCES}
    )

CollectIncludeDirectories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC_INCLUDES
    # Exclude
    ${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers)

target_include_directories(shared
    PUBLIC
    ${PUBLIC_INCLUDES}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(shared
    PRIVATE
    rendu-core-interface
    PUBLIC
    common
    )

set_target_properties(shared
    PROPERTIES
    FOLDER
    "server")

if(BUILD_SHARED_LIBS)
  if(UNIX)
    install(TARGETS shared
        LIBRARY
        DESTINATION lib)
  elseif(WIN32)
    install(TARGETS shared
        RUNTIME
        DESTINATION "${CMAKE_INSTALL_PREFIX}")
  endif()
endif()

# Generate precompiled header
if(USE_COREPCH)
  add_cxx_pch(shared ${PRIVATE_PCH_HEADER})
endif()

#|--------------------------------
#| rendu-core common project
#|--------------------------------

set(PROJECT_NAME common)

CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE_SOURCES
    # Exclude
#    ${CMAKE_CURRENT_SOURCE_DIR}/platform
)
CollectHeaderFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE_HEADERS
    # Exclude
#    ${CMAKE_CURRENT_SOURCE_DIR}/platform
)

if(USE_COREPCH)
  set(PRIVATE_PCH_HEADER precompiled_headers/common_pch.h)
endif(USE_COREPCH)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DRENDU_API_EXPORT_COMMON)

add_library(${PROJECT_NAME}
    ${PRIVATE_SOURCES}
    ${PRIVATE_HEADERS}
    )

CollectIncludeDirectories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC_INCLUDES
    # Exclude
    ${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    # Provide the binary dir for all child targets
    ${CMAKE_BINARY_DIR}
    ${PUBLIC_INCLUDES}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(common
    PRIVATE
    rendu-core-interface
    PUBLIC
    fmt
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER
    fmt::fmt
    )

if(BUILD_SHARED_LIBS)
  if(WINDOWS)
    install(TARGETS common
        LIBRARY
        DESTINATION lib)
  elseif(WIN32)
    install(TARGETS common
        RUNTIME
        DESTINATION "${CMAKE_INSTALL_PREFIX}")
  endif()
endif()

# Generate precompiled header
if(USE_COREPCH)
  add_cxx_pch(common ${PRIVATE_PCH_HEADER})
endif()

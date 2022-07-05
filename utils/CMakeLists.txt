# This file is part of the rendu-core Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
    UTIL_SOURCES
)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(utils ${UTILS_SOURCES})

target_link_libraries(utils
    PRIVATE
    trinity-core-interface
    # TODO：Add other libs
    )

CollectIncludeDirectories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    UTILS_INCLUDES)

target_include_directories(utils
    PUBLIC
    ${UTILS_INCLUDES}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

catch_discover_utils(utils)

set_target_properties(utils
    PROPERTIES
    FOLDER
    "utils")

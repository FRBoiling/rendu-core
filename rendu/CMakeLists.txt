
message(STATUS "Building rendu ...")
AddSubdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
    #    ${CMAKE_CURRENT_SOURCE_DIR}/gtest
    #    ${CMAKE_CURRENT_SOURCE_DIR}/catch2
)

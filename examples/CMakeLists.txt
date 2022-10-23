#|--------------------------------
#| rendu-core examples project
#|--------------------------------


#|--------------------------------
#| Build with examples
#|--------------------------------
if(RENDU_FLAG_EXAMPLES)
  message(STATUS "Building examples...")
  AddSubdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
      #    ${CMAKE_CURRENT_SOURCE_DIR}/spdlog
      #    ${CMAKE_CURRENT_SOURCE_DIR}/fmt
  )
endif()



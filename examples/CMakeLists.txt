#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core examples project
#|--------------------------------
message("")
if(RENDU_FLAG_EXAMPLES)
  message(STATUS "Building examples...")
  AddSubdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
      #    ${CMAKE_CURRENT_SOURCE_DIR}/spdlog
      #    ${CMAKE_CURRENT_SOURCE_DIR}/fmt
  )
endif()



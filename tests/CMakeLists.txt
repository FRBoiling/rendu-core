#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core tests module
#|--------------------------------
message("")
if(RENDU_FLAG_TESTS)
  message(STATUS "Building tests ...")
  AddSubdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
      #    ${CMAKE_CURRENT_SOURCE_DIR}/gtest
      #    ${CMAKE_CURRENT_SOURCE_DIR}/catch2
  )
endif()



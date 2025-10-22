#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core tests module
#|--------------------------------
if (RENDU_BUILD_TESTING)
  include(CTest)
  enable_testing()
  set(PROJECT_NAME test)
  rendu_add_subdirectories(
          DIR ${CMAKE_CURRENT_SOURCE_DIR}
          EXCLUDE_DIRS
  )
endif ()



#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core examples project
#|--------------------------------
if (RD_BUILD_EXAMPLES)
  set(PROJECT_NAME example_zltoolkit)
  rendu_add_subdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
  )
endif ()



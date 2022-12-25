#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core tests module
#|--------------------------------
if (RENDU_BUILD_TESTING)
  set(project-name tests)
  message("====== ${project-name} ======")
  rendu_add_subdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
  )

  message("")
endif ()



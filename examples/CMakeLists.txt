#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core examples project
#|--------------------------------
if (RENDU_FLAG_EXAMPLES)
  set(project-name examples)
  message("====== ${project-name} ======")
  rendu_add_subdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
  )

  message("")
endif ()



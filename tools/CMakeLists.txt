#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core tools module
#|--------------------------------
if (BUILD_TOOLS)
  set(project-name tools)
  message("====== ${project-name} ======")
  rendu_add_subdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
  )

  message("")
endif ()

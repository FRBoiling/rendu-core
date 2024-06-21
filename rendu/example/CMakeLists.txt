#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu common example projects
#|--------------------------------
  set(PROJECT_NAME rendu_example)
  rendu_add_subdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
      ${CMAKE_CURRENT_SOURCE_DIR}/core
      ${CMAKE_CURRENT_SOURCE_DIR}/net
      ${CMAKE_CURRENT_SOURCE_DIR}/task

  )

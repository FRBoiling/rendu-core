#**********************************
#  Created by boil on 2023/03/18.
#**********************************

if (RENDU_FLAG_EXAMPLES)
  set(PROJECT_NAME proto)
  rendu_add_subdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
  )
endif ()
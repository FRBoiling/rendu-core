#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core tools module
#|--------------------------------
if (BUILD_TOOLS)
  set(PROJECT_NAME tool)
  rendu_add_subdirectories(
      DIR ${CMAKE_CURRENT_SOURCE_DIR}
      EXCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/proto_generate
  )
endif ()

#**********************************
#  Created by boil on 2022/8/14.
#**********************************

if (CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
  add_definitions(-fno-delete-null-pointer-checks)
endif ()

AddSubdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

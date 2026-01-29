#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core rendu module
#|--------------------------------

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "正在配置 ${FOLDER_NAME} ")

# 首先添加 protocol 子目录(供所有应用共享)
add_subdirectory(protocol)

# 添加其他子目录
add_subdirectory(server)
add_subdirectory(client)

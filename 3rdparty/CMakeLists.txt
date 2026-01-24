#**********************************
#  Created by boil on 2022/8/14.
#**********************************

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
rendu_add_subdirectories(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        EXCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/absl
#        ${CMAKE_CURRENT_SOURCE_DIR}/argparse
#        ${CMAKE_CURRENT_SOURCE_DIR}/asio
#        ${CMAKE_CURRENT_SOURCE_DIR}/boost
        #        ${CMAKE_CURRENT_SOURCE_DIR}/catch
#        ${CMAKE_CURRENT_SOURCE_DIR}/entt
#        ${CMAKE_CURRENT_SOURCE_DIR}/fmt
        #        ${CMAKE_CURRENT_SOURCE_DIR}/gtest
        #        ${CMAKE_CURRENT_SOURCE_DIR}/protobuf
        ${CMAKE_CURRENT_SOURCE_DIR}/spdlog
        ${CMAKE_CURRENT_SOURCE_DIR}/threads
        ${CMAKE_CURRENT_SOURCE_DIR}/zlib
)
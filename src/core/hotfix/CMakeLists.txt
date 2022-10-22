#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target-name hotfix)
set(project-name core)

### 拷贝文件夹
#execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/proto/core/model
#    ${CMAKE_CURRENT_SOURCE_DIR})

CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
    private_sources
    # Exclude
)

add_library(${target-name}
    ${private_sources}
    )

CollectIncludeDirectories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    public_includes
    # Exclude
)

target_include_directories(${target-name}
    PUBLIC
    ${public_includes}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${target-name}
    PRIVATE
    rendu-core-interface
    PUBLIC
    libprotobuf
    #TODO:添加reps库
    )

set_target_properties(${target-name}
    PROPERTIES
    FOLDER
    ${project-name})

if (BUILD_SHARED_LIBS)
  if (UNIX)
    install(TARGETS model
        LIBRARY
        DESTINATION lib)
  elseif (WIN32)
    install(TARGETS model
        RUNTIME
        DESTINATION "${CMAKE_INSTALL_PREFIX}")
  endif ()
endif ()

#**********************************
#  Created by boil on 2022/8/14.
#**********************************
<<<<<<<< HEAD:example/libuv/pingpong_server/CMakeLists.txt
set(target_name pingpong_server)
========
set(target_name toolkit_echo_server)
>>>>>>>> f6e2ef7 (✨ feat(框架): 整理项目结构):example/zltoolkit/toolkit_echo_server/CMakeLists.txt
rendu_add_executable(
    DIR
    .
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${target_name}
    LINKOPTS
    rendu-core-interface
    DEPS
    dep::toolkit
    DEFINES
)


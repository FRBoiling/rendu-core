#**********************************
#  Created by boil on 2022/8/14.
#**********************************
# 阶段13：应用层 - 示例程序
# 演示框架各模块的使用方法
#**********************************

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "正在配置 ${FOLDER_NAME} 示例程序")

# 日志示例程序
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        log_example
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::common
        DEFINES
        ALIAS ON
        SOURCES
        log_example.cpp
)

# Actor 示例程序
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        actor_example
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::core
        DEFINES
        ALIAS ON
        SOURCES
        actor_example.cpp
        example_messages.h
)

# 引擎示例程序
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        engine_example
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::core
        DEFINES
        ALIAS ON
        SOURCES
        engine_example.cpp
)

# 事件示例程序
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        event_example
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::common
        DEFINES
        ALIAS ON
        SOURCES
        event_example.cpp
)

# ECS 示例程序
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        ecs_example
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::common
        DEFINES
        ALIAS ON
        SOURCES
        ecs_example.cpp
)

# 网络示例程序
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        net_example
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::common
        DEFINES
        ALIAS ON
        SOURCES
        net_example.cpp
)

# Metrics 示例程序
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        metrics_example
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::common
        DEFINES
        ALIAS ON
        SOURCES
        metrics_example.cpp
)

# Health 示例程序
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        health_example
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::common
        DEFINES
        ALIAS ON
        SOURCES
        health_example.cpp
)

# ActorRef 序列化示例程序
rendu_add_executable(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT
        ${PROJECT_NAME}
        NAME
        actor_ref_example
        PRIVATE_LINK
        rendu-core-interface
        PUBLIC_LINK RenduCore::core
        DEFINES
        ALIAS ON
        SOURCES
        actor_ref_example.cpp
)
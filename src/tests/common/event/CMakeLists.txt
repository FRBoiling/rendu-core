#**********************************
#  Created by boil on 2026/1/26.
#**********************************

# Event 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          event_test
        SOURCES       event_test.cpp
        PRIVATE_LINK  rendu-core-interface
        PUBLIC_LINK   3rdparty::catch RenduCore::common 3rdparty::boost
        TEST_FRAMEWORK CATCH2
)

# EventBus 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          event_bus_test
        SOURCES       event_bus_test.cpp
        PRIVATE_LINK  rendu-core-interface
        PUBLIC_LINK   3rdparty::catch RenduCore::common 3rdparty::boost
        TEST_FRAMEWORK CATCH2
)

# Handler 测试
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          handler_test
        SOURCES       handler_test.cpp
        PRIVATE_LINK  rendu-core-interface
        PUBLIC_LINK   3rdparty::catch RenduCore::common 3rdparty::boost
        TEST_FRAMEWORK CATCH2
)

# Handler 编译测试（验证 EventHandler 类型定义）
rendu_add_test(
        DIR           ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT       ${PROJECT_NAME}
        NAME          handler_compile_test
        SOURCES       handler_compile_test.cpp
        PRIVATE_LINK  rendu-core-interface
        PUBLIC_LINK   3rdparty::catch RenduCore::common 3rdparty::boost
        TEST_FRAMEWORK CATCH2
)

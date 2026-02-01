#**********************************
#  Created by boil on 2026/2/1.
#**********************************

# NodeDiscovery 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME node_discovery_test
        SOURCES node_discovery_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::core
        TEST_FRAMEWORK CATCH2
)

# LoadBalancer 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME load_balancer_test
        SOURCES load_balancer_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::core
        TEST_FRAMEWORK CATCH2
)

# FailoverManager 测试
rendu_add_test(
        DIR ${CMAKE_CURRENT_SOURCE_DIR}
        PROJECT ${PROJECT_NAME}
        NAME failover_manager_test
        SOURCES failover_manager_test.cpp
        PRIVATE_LINK rendu-core-interface Catch2::Catch2 Catch2::Catch2WithMain
        PUBLIC_LINK RenduCore::core
        TEST_FRAMEWORK CATCH2
)

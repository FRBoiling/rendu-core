#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(GTest CONFIG REQUIRED)
add_library(gtest INTERFACE)
target_link_libraries(gtest
    INTERFACE
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    )
add_library(dep::gtest ALIAS gtest)


#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(CAF CONFIG REQUIRED)
add_library(caf INTERFACE)
target_link_libraries(caf
    INTERFACE
    CAF::io
    CAF::core
    CAF::test
    CAF::openssl
    )
add_library(dep::caf ALIAS caf)
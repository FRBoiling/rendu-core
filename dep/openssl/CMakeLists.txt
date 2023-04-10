#**********************************
#  Created by boil on 2023/4/9.
#**********************************
#find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)
find_package(OpenSSL REQUIRED)
add_library(openssl INTERFACE)
target_link_libraries(openssl
    INTERFACE
    OpenSSL::Crypto
    OpenSSL::SSL
    )
add_library(dep::openssl ALIAS openssl)

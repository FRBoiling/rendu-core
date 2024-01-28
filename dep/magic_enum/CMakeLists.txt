#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(magic_enum CONFIG REQUIRED)
add_library(magic_enum INTERFACE)
target_link_libraries(magic_enum
    INTERFACE
    magic_enum::magic_enum
    )
add_library(dep::magic_enum ALIAS magic_enum)


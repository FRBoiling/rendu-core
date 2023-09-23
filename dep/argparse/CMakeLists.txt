#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(argparse REQUIRED)
add_library(argparse INTERFACE)
target_link_libraries(argparse
    INTERFACE
    argparse::argparse
    )
add_library(dep::argparse ALIAS argparse)

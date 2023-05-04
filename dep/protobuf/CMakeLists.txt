#**********************************
#  Created by boil on 2023/4/9.
#**********************************
find_package(Protobuf CONFIG REQUIRED)
add_library(protobuf INTERFACE)
target_link_libraries(protobuf
    INTERFACE
    protobuf::libprotoc
    protobuf::libprotobuf
    protobuf::libprotobuf-lite
    )
add_library(dep::protobuf ALIAS protobuf)

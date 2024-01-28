  #**********************************
  #  Created by boil on 2023/4/9.
  #**********************************

  find_package(libuv CONFIG REQUIRED)

  add_library(dep_libuv INTERFACE)
  target_link_libraries(dep_libuv
      INTERFACE
      $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>
  )
  add_library(dep::libuv ALIAS dep_libuv)

#**********************************
#  Created by boil on 2023/4/9.
#**********************************

find_package(SDL2 CONFIG REQUIRED)
add_library(sdl INTERFACE)
target_link_libraries(sdl
        INTERFACE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        )
add_library(dep::sdl ALIAS sdl)
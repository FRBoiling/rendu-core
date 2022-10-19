#**********************************
#  Created by boil on 2022/9/10.
#**********************************

set(target-name zlib)
set(project-name deps)

set(ZLIB_PUBLIC_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/zconf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/zlib.h
    )
set(ZLIB_PRIVATE_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/crc32.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/deflate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gzguts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inffast.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inffixed.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inflate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inftrees.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/trees.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/zutil.h
    )
set(ZLIB_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adler32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/crc32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/deflate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gzclose.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gzlib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gzread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gzwrite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inflate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/infback.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inftrees.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inffast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/trees.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uncompr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/zutil.c
    )

# parse the full version number from zlib.h and src in ZLIB_FULL_VERSION
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/src/zlib.h _zlib_h_contents)
string(REGEX REPLACE ".*#define[ \t]+ZLIB_VERSION[ \t]+\"([-0-9A-Za-z.]+)\".*"
    "\\1" ZLIB_FULL_VERSION ${_zlib_h_contents})

add_library(${target-name} SHARED ${ZLIB_SRCS} ${ZLIB_DLL_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
add_library(${target-name}static STATIC ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
set_target_properties(${target-name} PROPERTIES DEFINE_SYMBOL ZLIB_DLL)

if(UNIX)
    # On unix-like platforms the library is almost always called libz
    set_target_properties(${target-name} ${target-name}static PROPERTIES OUTPUT_NAME z)
    if(NOT APPLE)
        set_target_properties(${target-name} PROPERTIES LINK_FLAGS "-Wl,--version-script,\"${CMAKE_CURRENT_SOURCE_DIR}/zlib.map\"")
    endif()
elseif(BUILD_SHARED_LIBS AND WIN32)
    # Creates zlib1.dll when building shared library version
    set_target_properties(zlib PROPERTIES SUFFIX "1.dll")
endif()

target_compile_definitions(${target-name}
    INTERFACE
    -DRAPIDJSON_HAS_STDSTRING
    -DRAPIDJSON_48BITPOINTER_OPTIMIZATION=0
    -DRAPIDJSON_ASSERT=WPAssert
    )
include(CheckSymbolExists)
check_symbol_exists(fseeko "stdio.h" HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
    target_compile_definitions(${target-name} INTERFACE -DNO_FSEEKO)
endif()
check_include_file(unistd.h Z_HAVE_UNISTD_H)
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86_64)|(AMD64|amd64)|(^i.86$)")
    target_compile_definitions(${target-name} INTERFACE -DRAPIDJSON_SSE2)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    target_compile_definitions(${target-name} INTERFACE -DRAPIDJSON_NEON)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
    target_compile_definitions(${target-name} INTERFACE -DRAPIDJSON_NEON)
endif ()

target_link_libraries(${target-name}
    INTERFACE rendu-dependency-interface
    #    PUBLIC
    )
set_target_properties(${target-name} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${target-name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${target-name} PROPERTIES FOLDER ${project-name})

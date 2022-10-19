#**********************************
#  Created by boil on 2022/10/19.
#**********************************

#set(target-name spdlog)
set(project-name deps)

set(protobuf_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

get_filename_component(protobuf_source_dir ${protobuf_SOURCE_DIR} PATH)

# Options
option(protobuf_INSTALL "Install protobuf binaries and files" ON)
if(WITH_PROTOC)
  set(protobuf_PROTOC_EXE ${WITH_PROTOC} CACHE FILEPATH "Protocol Buffer Compiler executable" FORCE)
endif()
option(protobuf_BUILD_PROTOC_BINARIES "Build libprotoc and protoc compiler" ON)
option(protobuf_BUILD_LIBPROTOC "Build libprotoc" OFF)
option(protobuf_DISABLE_RTTI "Remove runtime type information in the binaries" OFF)

if (BUILD_SHARED_LIBS)
  set(protobuf_BUILD_SHARED_LIBS_DEFAULT ON)
else (BUILD_SHARED_LIBS)
  set(protobuf_BUILD_SHARED_LIBS_DEFAULT OFF)
endif (BUILD_SHARED_LIBS)

option(protobuf_BUILD_SHARED_LIBS "Build Shared Libraries" ${protobuf_BUILD_SHARED_LIBS_DEFAULT})
include(CMakeDependentOption)

cmake_dependent_option(protobuf_MSVC_STATIC_RUNTIME "Link static runtime libraries" ON
    "NOT protobuf_BUILD_SHARED_LIBS" OFF)
set(protobuf_WITH_ZLIB_DEFAULT ON)
option(protobuf_WITH_ZLIB "Build with zlib support" ${protobuf_WITH_ZLIB_DEFAULT})
set(protobuf_DEBUG_POSTFIX "d"
    CACHE STRING "Default debug postfix")
mark_as_advanced(protobuf_DEBUG_POSTFIX)
# User options
include(cmake/protobuf-options.cmake)
# Overrides for option dependencies
if (protobuf_BUILD_PROTOC_BINARIES OR protobuf_BUILD_TESTS)
  set(protobuf_BUILD_LIBPROTOC ON)
endif ()

add_definitions(-DGOOGLE_PROTOBUF_CMAKE_BUILD)
add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DHAVE_ZLIB)

if (protobuf_DISABLE_RTTI)
  add_definitions(-DGOOGLE_PROTOBUF_NO_RTTI=1)
endif()

# CheckLinkerFlag module available in CMake >=3.18.
if(${CMAKE_VERSION} VERSION_GREATER 3.18 OR ${CMAKE_VERSION} VERSION_EQUAL 3.18)
  include(CheckLinkerFlag)
  check_linker_flag(CXX -Wl,--version-script=${CMAKE_CURRENT_BINARY_DIR}/cmaketest.map protobuf_HAVE_LD_VERSION_SCRIPT)
else()
  include(CheckCXXSourceCompiles)
  set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS} -Wl,--version-script=${CMAKE_CURRENT_BINARY_DIR}/cmaketest.map)
  check_cxx_source_compiles("
    int main() {
      return 0;
    }
  " protobuf_HAVE_LD_VERSION_SCRIPT)
  set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})
endif()

include(cmake/libprotobuf-lite.cmake)
include(cmake/libprotobuf.cmake)
if (protobuf_BUILD_LIBPROTOC)
  include(cmake/libprotoc.cmake)
endif (protobuf_BUILD_LIBPROTOC)
if (protobuf_BUILD_PROTOC_BINARIES)
  include(cmake/protoc.cmake)
  if (NOT DEFINED protobuf_PROTOC_EXE)
    set(protobuf_PROTOC_EXE protoc)
  endif (NOT DEFINED protobuf_PROTOC_EXE)
endif (protobuf_BUILD_PROTOC_BINARIES)

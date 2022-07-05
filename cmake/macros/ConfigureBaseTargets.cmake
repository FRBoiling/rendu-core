# This file is part of the rendu-core Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# An interface library to make the target com available to other targets
add_library(rendu-compile-option-interface INTERFACE)

# Use -std=c++11 instead of -std=gnu++11
set(CXX_EXTENSIONS OFF)

# An interface library to make the target features available to other targets
add_library(rendu-feature-interface INTERFACE)

target_compile_features(rendu-feature-interface
  INTERFACE
    cxx_std_17)

# An interface library to make the warnings level available to other targets
# This interface taget is set-up through the platform specific script
add_library(rendu-warning-interface INTERFACE)

# An interface used for all other interfaces
add_library(rendu-default-interface INTERFACE)
target_link_libraries(rendu-default-interface
  INTERFACE
    rendu-compile-option-interface
    rendu-feature-interface)

# An interface used for silencing all warnings
add_library(rendu-no-warning-interface INTERFACE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(rendu-no-warning-interface
    INTERFACE
      /W0)
else()
  target_compile_options(rendu-no-warning-interface
    INTERFACE
      -w)
endif()

# An interface library to change the default behaviour
# to hide symbols automatically.
add_library(rendu-hidden-symbols-interface INTERFACE)

# An interface amalgamation which provides the flags and definitions
# used by the dependency targets.
add_library(rendu-dependency-interface INTERFACE)
target_link_libraries(rendu-dependency-interface
  INTERFACE
    rendu-default-interface
    rendu-no-warning-interface
    rendu-hidden-symbols-interface)

# An interface amalgamation which provides the flags and definitions
# used by the core targets.
add_library(rendu-core-interface INTERFACE)
target_link_libraries(rendu-core-interface
  INTERFACE
    rendu-default-interface
    rendu-warning-interface)

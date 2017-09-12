cmake_minimum_required(VERSION 3.6)
project(utility_belt)
set (UBELT_LIB_NAME ubelt)

set (LIB_VERSION_MAJOR 0)
set (LIB_VERSION_MINOR 1)
set (LIB_VERSION_PATCH 0)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

set( LIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wextra")

file(GLOB_RECURSE SRC_C src/*.c)
file(GLOB_RECURSE SRC_H src/*.h)

# Build project core as a library
add_library(ubeltx STATIC ${SRC_C} ${SRC_H})
add_library(ubelts SHARED ${SRC_C} ${SRC_H})

# Enable CTests suite reporting
enable_testing ()
add_subdirectory("tests")

if (APPLE)
  find_library(cf_lib CoreFoundation)
  set(UB_LIBS ${cf_lib})

elseif(UNIX)
  # We good..
endif()

target_link_libraries(ubeltx ${UB_LIBS})
target_link_libraries(ubelts ${UB_LIBS})

include_directories("${CMAKE_SOURCE_DIR}/src")

install(TARGETS ubelts DESTINATION lib)
install(TARGETS ubeltx DESTINATION lib)

install(FILES ${SRC_H} DESTINATION include/ubelt)




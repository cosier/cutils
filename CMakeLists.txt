cmake_minimum_required(VERSION 3.6)
project(ubelt)

set (LIB_VERSION_MAJOR 0)
set (LIB_VERSION_MINOR 1)
set (LIB_VERSION_PATCH 0)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

set( LIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

file(GLOB_RECURSE SRC_LIB src/*.c src/*.h)

# Remove tmp files from source tree glob
foreach ( PATH ${SRC_LIB} )
  string (FIND ${PATH} "#" FOUND)
  if (NOT ${FOUND} EQUAL -1)
    list ( REMOVE_ITEM SRC_LIB ${PATH} )
  endif()
endforeach( PATH )

# Build project core as a library
add_library(ubelt STATIC ${SRC_LIB})

# Enable CTests suite reporting
enable_testing ()
add_subdirectory("tests")

include_directories("${CMAKE_SOURCE_DIR}/src")

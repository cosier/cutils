cmake_minimum_required(VERSION 3.6)
project(ubelt-tests)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

# Treat all warnings as intense errors.
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

file(GLOB_RECURSE HELPER_LIB helpers/*.c helpers/*.h)

# message ( FATAL_ERROR "HELPER_SRC: ${HELPER_LIB}" )
include_directories(
  "${CMAKE_SOURCE_DIR}/tests"
  "${CMAKE_SOURCE_DIR}/src"
  )

add_library(ub_tests STATIC ${HELPER_LIB})
# Test Executable that utilises the library.

add_executable(test-ubelt test_main.c)

# Alias a shorter name, shortcut.
set(lib ${UBELT_LIB_NAME})

# Link up main executable with our Library: libdimappio
target_link_libraries( test-ubelt ${lib}x ub_tests)

add_test(NAME test-ubelt COMMAND test-ubelt)


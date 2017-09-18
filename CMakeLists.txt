cmake_minimum_required(VERSION 3.6)
set (APP_LIB_NAME ubelt)

project(${APP_LIB_NAME}_project)

set (LIB_VERSION_MAJOR 0)
set (LIB_VERSION_MINOR 1)
set (LIB_VERSION_PATCH 0)

# This project uses C11 Standard
set( CMAKE_C_STANDARD 11 )

set( LIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Treat all warnings as intense errors.
if(NOT MSVC)
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wextra")
endif()

file(GLOB_RECURSE SRC_C src/*.c)
file(GLOB_RECURSE SRC_H src/*.h)

# Build project core as a library
add_library(${APP_LIB_NAME}x STATIC ${SRC_C} ${SRC_H})
add_library(${APP_LIB_NAME}s SHARED ${SRC_C} ${SRC_H})

set( ENABLE_TESTS $ENV{ENABLE_UB_TESTS} )

# Enable CTests suite reporting
if ("${ENABLE_TESTS}" STREQUAL "true")
  enable_testing ()
  add_subdirectory("tests")
endif()

if (APPLE)
  find_library(cf_lib CoreFoundation)
  set(UB_LIBS ${cf_lib})

elseif(UNIX)
  # We good..
endif()

target_link_libraries(${APP_LIB_NAME}x ${UB_LIBS})
target_link_libraries(${APP_LIB_NAME}s ${UB_LIBS})

include_directories("${CMAKE_SOURCE_DIR}/src")

install(TARGETS ${APP_LIB_NAME}s DESTINATION lib)
install(TARGETS ${APP_LIB_NAME}x DESTINATION lib)

install(FILES ${SRC_H} DESTINATION include/${APP_LIB_NAME})




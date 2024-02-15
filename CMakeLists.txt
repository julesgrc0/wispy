cmake_minimum_required(VERSION 3.8)
project(wispy)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)


option(BUILD_MINSIZE "Build a smaller executable" OFF)


add_subdirectory(lib/raylib)
add_subdirectory(lib/zlib)

include_directories(${CMAKE_SOURCE_DIR}/raylib/src)
include_directories(${CMAKE_SOURCE_DIR}/zlib)

file(GLOB_RECURSE SOURCES "src/*.c" "src/*.rc")

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
target_link_libraries(${PROJECT_NAME} raylib zlibstatic)

if(BUILD_MINSIZE)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND upx --ultra-brute $<TARGET_FILE:${PROJECT_NAME}>
  )
endif()
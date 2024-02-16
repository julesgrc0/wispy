cmake_minimum_required(VERSION 3.8)
project(wispy VERSION 0.0.4 LANGUAGES C)

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

include_directories(
    ${CMAKE_SOURCE_DIR}/lib/raylib/src
    ${CMAKE_SOURCE_DIR}/lib/zlib
)

file(GLOB_RECURSE SOURCES "src/*.c" "src/*.rc")

add_definitions(-DWISPY_VERSION="${PROJECT_VERSION}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D_DEBUG)
endif()


if(ANDROID)
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
  target_link_libraries(${PROJECT_NAME} PRIVATE raylib zlibstatic android log)
elseif(UNIX)
  add_executable(${PROJECT_NAME} ${SOURCES})
  target_link_libraries(${PROJECT_NAME} PRIVATE raylib zlibstatic pthread)
else()
  add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
  target_link_libraries(${PROJECT_NAME} PRIVATE raylib zlibstatic)
endif()

if(BUILD_MINSIZE)
  find_program(UPX upx)
  if(UPX)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${UPX} --ultra-brute $<TARGET_FILE:${PROJECT_NAME}>
    )
  else()
    message(WARNING "UPX not found. Skipping executable compression.")
  endif()
endif()

if(UNIX OR ANDROID)
  if(EXISTS ${CMAKE_SOURCE_DIR}/tools/resource.pack)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/tools/resource.pack $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
  else()
    message(WARNING "resource.pack file not found. Skipping copying.")
  endif()
endif()
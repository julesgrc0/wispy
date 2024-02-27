cmake_minimum_required(VERSION 3.8)
project("wispy" VERSION 0.0.5 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# disable raylib examples and tests
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)

# disable zlib examples and tests
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)

# disable json-c examples and tests
set(DOXYGEN_FOUND OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)


if(ANDROID)
  # change project name to main for libmain
  set(PROJECT_NAME main)

  # set android abi
  string(REGEX MATCH "android-([0-9]+)" _ ${ANDROID_PLATFORM} FORCE)
  set(ANDROID_API_VERSION ${CMAKE_MATCH_1} CACHE STRING "" FORCE)
  set(ANDROID_ARCH ${CMAKE_ANDROID_ARCH} CACHE STRING "" FORCE)
  set(PLATFORM "Android" CACHE STRING "" FORCE)

  # change resource output path
  set(RESOURCE_FILE ${CMAKE_SOURCE_DIR}/android/app/src/main/assets/resource.pack)
  
  # set android flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3  -flto")
  if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp -mfpu=vfpv3-d16")
  elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfix-cortex-a53-835769")
  endif()

elseif(UNIX)
  # change resource output path
  set(RESOURCE_FILE ${CMAKE_SOURCE_DIR}/out/resource.pack)
endif()

option(W_BUILD_MINSIZE "Build a smaller executable (Wispy)" OFF)
option(W_BUILD_ASSETS "Build assets (Wispy)" ON)

# add dependencies
add_subdirectory(libs/raylib)
add_subdirectory(libs/zlib)
add_subdirectory(libs/json-c)

# add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/libs/raylib/src
    ${CMAKE_SOURCE_DIR}/libs/zlib
    ${CMAKE_SOURCE_DIR}/libs/json-c
)

# pack assets to resource.pack
if(W_BUILD_ASSETS)
  add_custom_target(pack_assets
      COMMAND python ${CMAKE_SOURCE_DIR}/tools/pack_assets.py
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

# add sources
file(GLOB_RECURSE SOURCES "src/*.c")
if(MSVC)
    list(APPEND SOURCES "src/ressource.rc")
endif()

# add definitions
add_definitions(-DWISPY_VERSION="version ${PROJECT_VERSION}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D_DEBUG)
endif()


if(ANDROID)
  # set android flags
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
  add_definitions(-D__ANDROID__)
  
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
  target_link_libraries(${PROJECT_NAME} raylib zlibstatic android log)
elseif(UNIX)
  add_executable(${PROJECT_NAME} ${SOURCES})
  target_link_libraries(${PROJECT_NAME} PRIVATE raylib zlibstatic json-c pthread)
elseif(MSVC)
  add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
  target_link_libraries(${PROJECT_NAME} PRIVATE raylib zlibstatic json-c)
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

# compress executable with upx
if(W_BUILD_MINSIZE)
  find_program(UPX upx)
  if(UPX)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${UPX} --ultra-brute $<TARGET_FILE:${PROJECT_NAME}>
    )
  else()
    message(WARNING "UPX not found. Skipping executable compression.")
  endif()
endif()

# copy resource.pack to output directory
if(UNIX OR ANDROID)
  if(EXISTS ${CMAKE_SOURCE_DIR}/tools/resource.pack)
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/tools/resource.pack ${RESOURCE_FILE}
      )
  else()
    message(WARNING "resource.pack file not found. Skipping copying.")
  endif()
endif()
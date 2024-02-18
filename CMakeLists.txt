cmake_minimum_required(VERSION 3.8)
project("wispy" VERSION 0.0.5 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)

set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)

if(ANDROID)
  set(PROJECT_NAME main)

  string(REGEX MATCH "android-([0-9]+)" _ ${ANDROID_PLATFORM} FORCE)
  set(ANDROID_API_VERSION ${CMAKE_MATCH_1} CACHE STRING "" FORCE)
  set(ANDROID_ARCH ${CMAKE_ANDROID_ARCH} CACHE STRING "" FORCE)
  set(PLATFORM "Android" CACHE STRING "" FORCE)

  set(RESOURCE_FILE ${CMAKE_SOURCE_DIR}/android/app/src/main/assets/resource.pack)

  # https://github.com/raysan5/raylib/issues/3491
  if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp -mfpu=vfpv3-d16")
  elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfix-cortex-a53-835769")
  endif()
elseif(UNIX)
  set(RESOURCE_FILE ${CMAKE_SOURCE_DIR}/out/resource.pack)
endif()

option(BUILD_MINSIZE "Build a smaller executable" OFF)

if(ANDROID)
add_custom_target(pack_assets
    COMMAND python ${CMAKE_SOURCE_DIR}/tools/pack_assets.py menu_blur.android.fs=menu_blur.fs
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
else()
add_custom_target(pack_assets
    COMMAND python ${CMAKE_SOURCE_DIR}/tools/pack_assets.py menu_blur.android.fs=none
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
endif()


add_subdirectory(lib/raylib)
add_subdirectory(lib/zlib)

include_directories(
    ${CMAKE_SOURCE_DIR}/lib/raylib/src
    ${CMAKE_SOURCE_DIR}/lib/zlib
)

file(GLOB_RECURSE SOURCES "src/*.c")
if(MSVC)
  list(APPEND SOURCES "src/ressource.rc")
endif()

add_definitions(-DWISPY_VERSION="version ${PROJECT_VERSION}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D_DEBUG)
endif()

if(ANDROID)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
  add_definitions(-D__ANDROID__)
  
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
  target_link_libraries(${PROJECT_NAME} raylib zlibstatic android log)
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
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/tools/resource.pack ${RESOURCE_FILE}
      )
  else()
    message(WARNING "resource.pack file not found. Skipping copying.")
  endif()
endif()
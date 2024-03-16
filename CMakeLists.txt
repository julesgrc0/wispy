cmake_minimum_required(VERSION 3.8)
project("wispy" VERSION 0.0.8 LANGUAGES C)

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
set(HAVE_ARC4RANDOM OFF CACHE BOOL "" FORCE)


set(RESOURCE_PACK_DIR ${CMAKE_SOURCE_DIR}/tools/)
if(ANDROID)
  # change project name to main for libmain
  set(PROJECT_NAME main)

  # set android abi
  string(REGEX MATCH "android-([0-9]+)" _ ${ANDROID_PLATFORM} FORCE)
  set(ANDROID_API_VERSION ${CMAKE_MATCH_1} CACHE STRING "" FORCE)
  set(ANDROID_ARCH ${CMAKE_ANDROID_ARCH} CACHE STRING "" FORCE)
  set(PLATFORM "Android" CACHE STRING "" FORCE)

  # change resource output path
  set(RESOURCE_PACK_DIR ${CMAKE_SOURCE_DIR}/android/app/src/main/assets/)
  
  # set android flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3  -flto")
  if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp -mfpu=vfpv3-d16")
  elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfix-cortex-a53-835769")
  endif()
endif()

option(W_BUILD_MINSIZE "Build a smaller executable (Wispy)" OFF)

# add dependencies and include directories
file(GLOB SUBDIRS "libs/*")
foreach(DIR ${SUBDIRS})
  if(IS_DIRECTORY ${DIR})
    add_subdirectory(${DIR})
  endif()
endforeach()

# pack assets to resource.pack
if(ANDROID)
    execute_process(
            COMMAND python ${CMAKE_SOURCE_DIR}/tools/pack_assets.py ${CMAKE_SOURCE_DIR}/tools/mobile.json ${RESOURCE_PACK_DIR}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else()
    execute_process(
            COMMAND python ${CMAKE_SOURCE_DIR}/tools/pack_assets.py ${CMAKE_SOURCE_DIR}/tools/desktop.json ${RESOURCE_PACK_DIR}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()


# add sources
file(GLOB_RECURSE SOURCES "src/*.c")

# add definitions
add_definitions(-DWISPY_VERSION="version ${PROJECT_VERSION}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D_DEBUG)
endif()


if(ANDROID)
  # set android flags
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
  add_definitions(-D__ANDROID__)
  
  list(APPEND raylib_sources ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
  include_directories(${ANDROID_NDK}/sources/android/native_app_glue)

  add_library(${PROJECT_NAME} SHARED ${SOURCES})
  target_link_libraries(${PROJECT_NAME} raylib zlibstatic json-c android log)
elseif(UNIX AND NOT ANDROID)
  add_executable(${PROJECT_NAME} ${SOURCES})
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OBJCOPY_ARCH "elf64-x86-64")
  else()
    set(OBJCOPY_ARCH "elf32-i386")
  endif()

  add_custom_target(link_resource
  COMMAND  objcopy --prefix-symbol=w --localize-hidden --rename-section .data=.rdata,CONTENTS,ALLOC,LOAD,READONLY,DATA --input-target binary --binary-architecture i386 --output-target ${OBJCOPY_ARCH} ${RESOURCE_PACK_DIR}/resource.pack ${CMAKE_BINARY_DIR}/resource.o --verbose
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
  add_dependencies(${PROJECT_NAME} link_resource)
  target_link_libraries(${PROJECT_NAME} PRIVATE raylib zlibstatic json-c pthread ${CMAKE_BINARY_DIR}/resource.o)
elseif(WIN32)
  list(APPEND SOURCES "src/resource.rc")

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
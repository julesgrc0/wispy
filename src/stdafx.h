#pragma once
#ifndef _STDAFX_H
#define _STDAFX_H

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC

#include <math.h>
#include <memory.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/// ZLIB
#include <zlib.h>
///

/// RAYLIB
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
///

#ifdef _WIN32

#define NOGDI
#define NOUSER

#include <crtdbg.h>
#include <crtdefs.h>
#include <windows.h>

/// OpenMP
#include <omp.h>
///

#undef near
#undef far

#define MAKEINTRESOURCE(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))

#elif __linux__
#define MAX_PATH 260

#include <pthread.h>
#include <unistd.h>

#elif __ANDROID__

#include <jni.h>
#include <android/log.h>
#include <pthread.h>

#endif

#define sfree(x)                                                               \
  if (x)                                                                       \
    free(x);

#define PHYSICS_TICK (1.0f / 120.0f)

#define RENDER_CUBE_COUNT 20
#define RENDER_CUBE_GAP 2
#define RENDER_W 1280
#define RENDER_H 720

#define CHUNK_MID_H 128
#define CHUNK_H 256 // 2^8
#define CHUNK_W 64  // 2^6

#define CHUNK_GROUP_LEN 10
#define CHUNK_GROUP_UPDATE_DIST 2
#define CHUNK_GROUP_MID_LEN 5
#define CHUNK_GROUP_LOAD 2

#define CUBE_W (RENDER_W / RENDER_CUBE_COUNT)
#define CUBE_H (RENDER_H / RENDER_CUBE_COUNT)

#define FULL_CHUNK_W (CHUNK_W * CUBE_W)
#define FULL_CHUNK_H (CHUNK_H * CUBE_H)

#define BLOCK_BRUTE_VALUE_MAX 255
#define BLOCK_TOP_LAYER_H 40

#define VEC_ZERO                                                               \
  (Vector2) { 0 }

#if defined(_DEBUG) && (defined(__linux__) || defined(_WIN32))
#define LOG(...)                                                               \
  printf("INFO: ");                                                            \
  printf(__VA_ARGS__);                                                         \
  printf("\n");
#elif defined(__ANDROID__)
#define LOG(...)                                                               \
  __android_log_print(ANDROID_LOG_INFO, "INFO", __VA_ARGS__);
#else
#define LOG(...)
#endif // _DEBUG

#if defined(_DEBUG) && defined(_WIN32)
#define measure(name, x)                                                       \
  do {                                                                         \
    LARGE_INTEGER start, end, frequency;                                       \
    long long elapsed_time;                                                    \
    QueryPerformanceFrequency(&frequency);                                     \
    QueryPerformanceCounter(&start);                                           \
                                                                               \
    x;                                                                         \
                                                                               \
    QueryPerformanceCounter(&end);                                             \
    elapsed_time =                                                             \
        (end.QuadPart - start.QuadPart) * 100000000 / frequency.QuadPart;      \
    printf("[%s]: %lld ns\n", #name, elapsed_time);                            \
  } while (0);

#elif defined(_DEBUG) && (defined(__linux__)  || defined(__ANDROID__))
#define measure(name, x)                                                       \
  do {                                                                         \
    struct timespec start, end;                                                \
    long long elapsed_time;                                                    \
    clock_gettime(CLOCK_MONOTONIC, &start);                                     \
                                                                               \
    x;                                                                         \
                                                                               \
    clock_gettime(CLOCK_MONOTONIC, &end);                                       \
    elapsed_time =                                                             \
        (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);\
    printf("[%s]: %lld ns\n", #name, elapsed_time);                            \
  } while (0);
#else
#define measure(name, x)
#endif // _DEBUG && _WIN32

#endif // _STDAFX_H
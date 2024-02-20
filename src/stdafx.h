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

#include <android/log.h>
#include <jni.h>
#include <pthread.h>

#endif

#ifndef WISPY_VERSION
#define WISPY_VERSION "version ?.?.?"
#endif

#define sfree(x)                                                               \
  if (x)                                                                       \
    free(x);

#define PERCENT_W(x) RENDER_W *x
#define PERCENT_H(x) RENDER_H *x
#define PIXELS_W(x) RENDER_W / x
#define PIXELS_H(x) RENDER_H / x

#define FORMAT_TO(x, size, target) (((float)x / (float)size) * (float)target)
#define FORMAT_W(x) FORMAT_TO(x, GetRenderWidth(), RENDER_W)
#define FORMAT_H(x) FORMAT_TO(x, GetRenderHeight(), RENDER_H)
#define FORMAT_VEC(x)                                                          \
  (Vector2) { FORMAT_W(x.x), FORMAT_H(x.y) }

#define PHYSICS_TICK (1.0f / 120.0f)

#define RENDER_W 1280
#define RENDER_H 720

#define RENDER_CUBE_COUNT 20
#define RENDER_CUBE_GAP 2

#define CHUNK_H 256 // 2^8
#define CHUNK_W 64  // 2^6
#define CHUNK_MID_H 128

#define CHUNK_GROUP_LEN 10
#define CHUNK_GROUP_UPDATE_DIST 2
#define CHUNK_GROUP_MID_LEN 5

#define CUBE_W (RENDER_W / RENDER_CUBE_COUNT)
#define CUBE_H (RENDER_H / RENDER_CUBE_COUNT)

#define FULL_CHUNK_W (CHUNK_W * CUBE_W)
#define FULL_CHUNK_H (CHUNK_H * CUBE_H)

#define BLOCK_TOP_LAYER_H 40

#define VEC_ZERO                                                               \
  (Vector2) { 0 }
#define VEC(...)                                                               \
  (Vector2) { __VA_ARGS__ }

#if defined(_DEBUG) && (defined(__linux__) || defined(_WIN32))
#define LOG(...)                                                               \
  printf("INFO: ");                                                            \
  printf(__VA_ARGS__);                                                         \
  printf("\n");
#elif defined(_DEBUG) && defined(__ANDROID__)
#define LOG(...) __android_log_print(ANDROID_LOG_INFO, "WISPY", __VA_ARGS__);
#else
#define LOG(...)
#endif // _DEBUG

#endif // _STDAFX_H
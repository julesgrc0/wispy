#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC

#include <math.h>
#include <memory.h>
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
#else
#define MAX_PATH 260

#include <pthread.h>
#include <unistd.h>

#endif

#define sfree(x) \
  if (x)         \
    free(x);

#define RENDER_W 1280
#define RENDER_H 720


#ifdef _DEBUG
#define LOG(...) printf(__VA_ARGS__)
#define LOGIF(cond, ...) \
  if (cond)              \
  printf(__VA_ARGS__)
#else
#define LOG(...)
#define LOGIF(cond, ...)
#endif // _DEBUG


#if defined(_DEBUG) && defined(_WIN32)
#define measure(name, x)                                                \
  LARGE_INTEGER start, end, frequency;                                  \
  long long elapsed_time;                                               \
  QueryPerformanceFrequency(&frequency);                                \
  QueryPerformanceCounter(&start);                                      \
  x;                                                                    \
  QueryPerformanceCounter(&end);                                        \
  elapsed_time =                                                        \
      (end.QuadPart - start.QuadPart) * 100000000 / frequency.QuadPart; \
  printf("[%s]: %lld ns\n", #name, elapsed_time);

#else
#define measure(name, x)
#endif // _DEBUG && _WIN32

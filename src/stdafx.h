#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <memory.h>

#include <crtdbg.h>
#include <crtdefs.h>


//vcpkg install raylib:x86-windows-static  raylib:x64-windows-static
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

//vcpkg install zlib:x86-windows-static  zlib:x64-windows-static
#include <zlib.h>

#if defined(_WIN32)
#define NOGDI
#define NOUSER 
#endif

#include <windows.h>

#if defined(_WIN32) 
#undef near
#undef far

#define MAKEINTRESOURCE(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#endif

#define sfree(x) if(x) free(x);
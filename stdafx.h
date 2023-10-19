#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
//#define _WIN32_WINDOWS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>

//vcpkg install raylib:x86-windows-static  raylib:x64-windows-static
#include<raylib.h>
#include<raymath.h>
#include<rlgl.h>


#if defined(_WIN32)           
#define NOGDI
#define NOUSER
#endif

#include<windows.h>

#if defined(_WIN32)        
#undef near
#undef far
#endif

#include <crtdbg.h>
#include <crtdefs.h>

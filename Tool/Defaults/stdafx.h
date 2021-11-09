// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma warning(disable: 4005)

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <d3d11.h>

#include <map>
#include <string>
#include <iostream>
#include <experimental\filesystem>

// TODO: reference additional headers your program requires here
#include "Engine.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo.h"

using namespace std;

extern HWND		g_hWnd;
extern _bool	g_Done;

#define FILESYSTEM std::experimental::filesystem
